typedef struct {
   volatile unsigned int MODER;
   volatile unsigned int OTYPER;
   volatile unsigned int OSPEEDR;
   volatile unsigned int PUPDR;
   volatile unsigned int IDR;
   volatile unsigned int ODR;
   volatile unsigned int BSRR;
   volatile unsigned int LCKR;
   volatile unsigned int ARF[2];
   volatile unsigned int BRR;
}GPIO_TypeDef;
#define PERIPH_BASE (0x40000000)
#define AHBPERIPH_HASE (PERIPH_BASE + 0x20000)
#define RCC_BASE (AHBPERIPH_HASE + 0x1000)
#define RCC_IOPENR_OFFSET 0x2c
#define RCC_IOPENR_ADDR *(volatile unsigned int*)(RCC_BASE+RCC_IOPENR_OFFSET )

#define RCC_GPIOA_EN ((unsigned int)(0x00000001U))
#define RCC_GPIOB_EN ((unsigned int)(0x00000002U))
#define IOPAEN_BIT 1
#define IOPBEN_BIT 2
#define IOPCEN_BIT 4

#define GPIO_ADDR (unsigned int) 0x50000000U
#define GPIO_PORTA_BASE (GPIO_ADDR)
#define GPIOA (GPIO_TypeDef*)GPIO_PORTA_BASE
#define GPIO_PORTB_BASE ((GPIO_ADDR) + 0x400U)
#define GPIOB (GPIO_TypeDef*)GPIO_PORTB_BASE

#define DELAY_TIME 0x10000
void delay ( unsigned int t ){
   volatile int counter=0;
   while(counter <t){
      ++counter;
   }
   return;
}
#define GPIO_2BIT_POS_MASK ((unsigned int)0x00000003U) //0b11
#define GPIO_1BIT_POS_MASK ((unsigned int)0x00000001U)//0b01
#define GPIO_PIN_5_POS 5
#define GPIO_PIN_8_POS 8
//d7 - pa8
typedef enum{
   GPIO_PIN_RESET = 0U,
   GPIO_PIN_SET
}GPIO_PinState;
void GPIO_Init(GPIO_TypeDef* port, unsigned short pin, GPIO_TypeDef*initVal);
void GPIO_Write_Pin(GPIO_TypeDef* port, unsigned short pin, GPIO_PinState state);
//void GPIO_Read_Pin(GPIO_TypeDef* port, unsigned short pin, GPIO_PinState state);
void GPIO_Toggle_Pin(GPIO_TypeDef* port, unsigned short pin){
   unsigned int position = pin;
   position = GPIO_1BIT_POS_MASK << pin;
   unsigned int odr;
   odr = port -> ODR;
   
   port -> BSRR = ((odr&position)<<16)| (~odr& position);
}
void GPIO_Write_Pin(GPIO_TypeDef* port, unsigned short pin, GPIO_PinState state){
      //5. ourput data(ODR) - 1bit
   /*unsigned int reg = port -> ODR;
   reg &= ~(GPIO_1BIT_POS_MASK << pin);
   unsigned int tmp = reg;
   reg |= state << pin;
   port-> ODR = reg;*/
   
   //BSRR/BRR
   unsigned int position =0;
   position = GPIO_1BIT_POS_MASK << pin;
   if( state != GPIO_PIN_RESET){
      port->BSRR = position;
   } else {
      port->BRR = position;
   }
   
}
void GPIO_init(GPIO_TypeDef* port, unsigned short pin, GPIO_TypeDef* initVal){
   //1. GPIO port mode register (GPIOx_MODER)
   //GPIO_MODE_REG_ADDR = 0xEBFFF4FF;
   unsigned int reg =  port -> MODER ;
   reg &= ~(GPIO_2BIT_POS_MASK << 2U*pin);
   reg |= ((initVal->MODER) << 2U*pin);
   port -> MODER  = reg;

   //2. type register - 1bit
   //GPIO_TYPE_REG_ADDR = 0x0;
   reg = port -> OTYPER ;
   reg &= ~(GPIO_1BIT_POS_MASK << 1U*pin);
   reg |= ( (initVal->OTYPER)  << 1U*pin);
   port -> OTYPER = reg;
   
   //3. output speed register - 2bit
   //GPIO_OUTPUT_SPEED_REG_ADDR = 0x0c000c00;
   reg = port -> OSPEEDR;
   reg &= ~(GPIO_2BIT_POS_MASK << 2U*pin);
   reg |= ( (initVal->OSPEEDR) << 2U*pin);
   port -> OSPEEDR = reg;
   
   //4. pull up / down - 2bit
   //GPIO_PULL_UP_DOWN_REG_ADDR = 0x24000000;
   reg = port -> PUPDR;
   reg &= ~(GPIO_2BIT_POS_MASK << 2U*pin);
   reg |= ((initVal->PUPDR)  << 2U*pin);
   port -> PUPDR = reg;
}   

int main (void){
/*
1. Rcc port a enable register (IOPAEN)
AHB
0x40021000
I/O port
0x2c offset
= 0x 4002 102c 
*/
   
   RCC_IOPENR_ADDR |= RCC_GPIOA_EN;
   RCC_IOPENR_ADDR |= RCC_GPIOB_EN;
   GPIO_TypeDef* GPIOA_reg = GPIOA;
   GPIO_TypeDef* GPIOB_reg = GPIOB;
   GPIO_TypeDef init_val;
   init_val.MODER = 0x000000001U;
   init_val.OTYPER = 0x000000000U;
   init_val.OSPEEDR = 0x00000003U;
   init_val.PUPDR = 0x00000000U;
   GPIO_init(GPIOA_reg, GPIO_PIN_5_POS, &init_val);
   GPIO_init(GPIOA_reg, GPIO_PIN_8_POS, &init_val);
   GPIO_init(GPIOB_reg, GPIO_PIN_5_POS, &init_val);
   GPIO_PinState GPIOA_PIN_5_State = GPIO_PIN_RESET;
   GPIO_PinState GPIOA_PIN_8_State = GPIO_PIN_SET;
   GPIO_PinState GPIOB_PIN_5_State = GPIO_PIN_RESET;
   GPIO_Write_Pin(GPIOA_reg, GPIO_PIN_5_POS, GPIOA_PIN_5_State);
   GPIO_Write_Pin(GPIOA_reg, GPIO_PIN_8_POS, GPIOA_PIN_8_State);
   GPIO_Write_Pin(GPIOB_reg, GPIO_PIN_5_POS, GPIOB_PIN_5_State);
   
   while (1){
      delay(DELAY_TIME);
      /*
      GPIOA_PIN_5_State = (GPIO_PinState)(!GPIOA_PIN_5_State);
      GPIOA_PIN_8_State = (GPIO_PinState)(!GPIOA_PIN_8_State);
      GPIOB_PIN_5_State = (GPIO_PinState)(!GPIOB_PIN_5_State);
      GPIO_Write_Pin(GPIOA_reg, GPIO_PIN_5_POS, GPIOA_PIN_5_State);
      GPIO_Write_Pin(GPIOA_reg, GPIO_PIN_8_POS, GPIOA_PIN_8_State);
      GPIO_Write_Pin(GPIOB_reg, GPIO_PIN_5_POS, GPIOB_PIN_5_State);*/
      GPIO_Toggle_Pin(GPIOA_reg, GPIO_PIN_5_POS);
      GPIO_Toggle_Pin(GPIOA_reg, GPIO_PIN_8_POS);
      GPIO_Toggle_Pin(GPIOB_reg, GPIO_PIN_5_POS);
   }
   return 0; 
}