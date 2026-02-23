#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define GPIO_BASE   (0xFE200000) // GPIO controller address
#define GPIO_SIZE   (256)

#define GPIO_IN(g)  (*(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))) // input
#define GPIO_OUT(g) (*(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3)))  // output

#define GPIO_SET(g) (*(gpio + 7) = 1 << g)	  // bit set
#define GPIO_CLR(g) (*(gpio + 10) = 1 << g)	  // bit clear
#define GPIO_GET(g) (*(gpio + 13) & (1 << g)) // get current GPIO infomation

volatile unsigned *gpio;

#define SEG_MAJOR  201
#define SEG_MINOR  0
#define SEG_DEVICE "gpioSeg"

#define SEG_A    7
#define SEG_B    1
#define SEG_C    19
#define SEG_D    13
#define SEG_E    6
#define SEG_F    8
#define SEG_G    25
#define SEG_DOT  26

static const int seg_pin[8] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DOT };

static char msg[BLOCK_SIZE] = {0};

static int seg_open(struct inode *, struct file *);
static ssize_t seg_read(struct file *, char *, size_t, loff_t *);
static ssize_t seg_write(struct file *, const char *, size_t, loff_t *);
static int seg_close(struct inode *, struct file *);

static struct file_operations seg_fops = {
    .owner   = THIS_MODULE,
    .read    = seg_read,
    .write   = seg_write,
    .open    = seg_open,
    .release = seg_close,
};

static struct cdev seg_cdev;

static const unsigned char hex_pat[10] = {
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110, // 9
};

static inline void seg_onoff(int pin, int on) { on ? GPIO_CLR(pin) : GPIO_SET(pin); }
static void seg_all_off(void) { for (int i = 0; i < 8; ++i) seg_onoff(seg_pin[i], 0); }
static void seg_apply_pattern(unsigned char p) {
    seg_onoff(SEG_A,  (p & 0b10000000) != 0);
    seg_onoff(SEG_B,  (p & 0b01000000) != 0);
    seg_onoff(SEG_C,  (p & 0b00100000) != 0);
    seg_onoff(SEG_D,  (p & 0b00010000) != 0);
    seg_onoff(SEG_E,  (p & 0b00001000) != 0);
    seg_onoff(SEG_F,  (p & 0b00000100) != 0);
    seg_onoff(SEG_G,  (p & 0b00000010) != 0);
    seg_onoff(SEG_DOT, (p & 0b00000001) != 0);
}
static int char_to_hex(int c) {
    if ('0' <= c && c <= '9') return c - '0';
    return -1;
}

int init_module(void) {
    dev_t devno;
    unsigned int count;
    static void *map;
    int err;
    int i;

    printk(KERN_INFO "Hello 7-seg module!\n");

    devno = MKDEV(SEG_MAJOR, SEG_MINOR);
    err = register_chrdev_region(devno, 1, SEG_DEVICE);
    if (err < 0) {
        printk("Error : register_chrdev_region\n");
        return err;
    }

    cdev_init(&seg_cdev, &seg_fops);
    seg_cdev.owner = THIS_MODULE;
    count = 1;

    err = cdev_add(&seg_cdev, devno, count);
    if (err < 0) {
        printk("Error : Device Add\n");
        unregister_chrdev_region(devno, 1);
        return -1;
    }

    printk("'mknod /dev/%s c %d 0'\n", SEG_DEVICE, SEG_MAJOR);
    printk("'chmod 666 /dev/%s'\n", SEG_DEVICE);

    map = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!map) {
        printk("Error : mapping GPIO memory\n");
        cdev_del(&seg_cdev);
        unregister_chrdev_region(devno, 1);
        return -EBUSY;
    }
    gpio = (volatile unsigned int *)map;

    for (i = 0; i < 8; ++i) {
        GPIO_IN(seg_pin[i]);
        GPIO_OUT(seg_pin[i]);
    }

    seg_all_off();
    return 0;
}

void cleanup_module(void) {
    dev_t devno = MKDEV(SEG_MAJOR, SEG_MINOR);

    seg_all_off();

    cdev_del(&seg_cdev);
    unregister_chrdev_region(devno, 1);
    if (gpio) iounmap(gpio);
}

static int seg_open(struct inode *inod, struct file *fil) {
    printk("7SEG Device opened(%d/%d)\n", imajor(inod), iminor(inod));
    return 0;
}

static int seg_close(struct inode *inod, struct file *fil) {
    printk("7SEG Device closed(%d/%d)\n", imajor(inod), iminor(inod));
    return 0;
}

static ssize_t seg_read(struct file *inode, char *buff, size_t len, loff_t *off) {
    const char *kmsg = "sevenseg\n";
    size_t klen = strlen(kmsg) + 1;

    if (len < klen) klen = len;

    if (copy_to_user(buff, kmsg, klen))
        return -EFAULT;

    return (ssize_t)klen;
}

static ssize_t seg_write(struct file *inode, const char *buff, size_t len, loff_t *off) {
    size_t n;
    int h;

    if (len == 0) return 0;

    memset(msg, 0, BLOCK_SIZE);

    n = (len >= (BLOCK_SIZE - 1)) ? (BLOCK_SIZE - 1) : len;
    if (copy_from_user(msg, buff, n))
        return -EFAULT;

    if (msg[0] == 'x' || msg[0] == 'X') {
        seg_all_off();
    } else if (msg[0] == '.') {
        seg_apply_pattern(0b00000001);
    } else {
        h = char_to_hex(msg[0]);
        if (h >= 0) seg_apply_pattern(hex_pat[h]);
        else        seg_all_off();
    }

    printk("7SEG write : '%c' (len=%zu)\n", msg[0], len);
    return (ssize_t)len;
}