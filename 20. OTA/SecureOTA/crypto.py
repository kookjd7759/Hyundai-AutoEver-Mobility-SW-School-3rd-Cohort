import os
import hashlib
from pathlib import Path

from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.padding import PKCS7


# ============================================================
# Common Settings
# ============================================================

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

AES_BLOCK_SIZE = algorithms.AES.block_size  # 128-bit
AES_IV_SIZE = 16
RSA_KEY_SIZE = 4096
RSA_PUBLIC_EXPONENT = 65537
PRIVATE_KEY_PASSWORD = b"private"
DEFAULT_KEY_DIR = os.path.join(BASE_DIR,"key")
DEFAULT_PRIVATE_KEY_NAME = "Private_key.pem"
DEFAULT_PUBLIC_KEY_NAME = "Public_key.pem"


# ============================================================
# Internal Helper Functions
# ============================================================

def _load_public_key(public_key_path):
    with open(public_key_path, "rb") as key_file:
        return serialization.load_pem_public_key(key_file.read())


def _load_private_key(private_key_path, password=PRIVATE_KEY_PASSWORD):
    with open(private_key_path, "rb") as key_file:
        return serialization.load_pem_private_key(
            key_file.read(),
            password=password
        )


def _rsa_oaep_padding():
    return padding.OAEP(
        mgf=padding.MGF1(algorithm=hashes.SHA256()),
        algorithm=hashes.SHA256(),
        label=None
    )


def _rsa_pss_padding():
    return padding.PSS(
        mgf=padding.MGF1(hashes.SHA256()),
        salt_length=padding.PSS.MAX_LENGTH
    )


# ============================================================
# AES-CBC Encryption / Decryption
# ============================================================

def encrypt_file_aes(file_data, aes_key):
    """
    Encrypt bytes using AES-CBC + PKCS7 padding.

    Args:
        file_data (bytes): Plain data to encrypt.
        aes_key (bytes): AES key. Must be 16, 24, or 32 bytes.

    Returns:
        tuple[bytes, bytes]: iv, cipher_text
    """
    padder = PKCS7(AES_BLOCK_SIZE).padder()
    padded_data = padder.update(file_data) + padder.finalize()

    iv = os.urandom(AES_IV_SIZE)
    cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv))
    encryptor = cipher.encryptor()
    cipher_text = encryptor.update(padded_data) + encryptor.finalize()

    return iv, cipher_text


def decrypt_file_aes(cipher_text, aes_key, iv):
    """
    Decrypt AES-CBC encrypted bytes and remove PKCS7 padding.

    Args:
        cipher_text (bytes): Encrypted data.
        aes_key (bytes): AES key. Must be 16, 24, or 32 bytes.
        iv (bytes): AES-CBC IV. Must be 16 bytes.

    Returns:
        bytes: Decrypted plain data.
    """
    try:
        cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv))
        decryptor = cipher.decryptor()
        padded_data = decryptor.update(cipher_text) + decryptor.finalize()

        unpadder = PKCS7(AES_BLOCK_SIZE).unpadder()
        return unpadder.update(padded_data) + unpadder.finalize()

    except Exception as e:
        raise ValueError(f"AES decryption failed: {e}") from e


# ============================================================
# RSA Key Generation / Encryption / Decryption
# ============================================================

def rsa_key_generation(key_dir=DEFAULT_KEY_DIR, password=PRIVATE_KEY_PASSWORD):
    """
    Generate RSA private/public key pair and save them as PEM files.

    Args:
        key_dir (str): Directory path where PEM files will be saved.
        password (bytes): Password used to encrypt private key PEM.

    Returns:
        tuple[str, str]: private_key_path, public_key_path
    """
    private_key = rsa.generate_private_key(
        public_exponent=RSA_PUBLIC_EXPONENT,
        key_size=RSA_KEY_SIZE
    )
    public_key = private_key.public_key()

    private_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.BestAvailableEncryption(password)
    )

    public_pem = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )

    key_path = Path.cwd() / key_dir
    key_path.mkdir(parents=True, exist_ok=True)

    private_key_path = key_path / DEFAULT_PRIVATE_KEY_NAME
    public_key_path = key_path / DEFAULT_PUBLIC_KEY_NAME

    private_key_path.write_bytes(private_pem)
    public_key_path.write_bytes(public_pem)

    return str(private_key_path), str(public_key_path)


def encrypt_file_rsa(file_data, public_key_path):
    """
    Encrypt bytes using RSA-OAEP with SHA-256.

    Note:
        RSA can only encrypt small data. For large firmware/file data,
        encrypt the file with AES and encrypt only the AES key with RSA.
    """
    public_key = _load_public_key(public_key_path)
    return public_key.encrypt(file_data, _rsa_oaep_padding())


def decrypt_file_rsa(encrypted_data, private_key_path, password=PRIVATE_KEY_PASSWORD):
    """
    Decrypt RSA-OAEP encrypted bytes.
    """
    private_key = _load_private_key(private_key_path, password=password)
    return private_key.decrypt(encrypted_data, _rsa_oaep_padding())


# ============================================================
# Hash
# ============================================================

def compute_file_hash(file_path):
    """
    Compute SHA-256 hash of a file.

    Args:
        file_path (str): Target file path.

    Returns:
        str: SHA-256 hex digest.
    """
    sha256_hash = hashlib.sha256()

    with open(file_path, "rb") as file:
        for chunk in iter(lambda: file.read(4096), b""):
            sha256_hash.update(chunk)

    return sha256_hash.hexdigest()


# ============================================================
# RSA Signature / Verification
# ============================================================

def sign_file(file_data, private_key_path, password=PRIVATE_KEY_PASSWORD):
    """
    Sign bytes using RSA-PSS with SHA-256.
    """
    private_key = _load_private_key(private_key_path, password=password)

    return private_key.sign(
        file_data,
        _rsa_pss_padding(),
        hashes.SHA256()
    )


def verify_sign(signature, file_data, public_key_path):
    """
    Verify RSA-PSS signature.

    Returns:
        bool: True if signature is valid, otherwise False.
    """
    public_key = _load_public_key(public_key_path)

    try:
        public_key.verify(
            signature,
            file_data,
            _rsa_pss_padding(),
            hashes.SHA256()
        )
        return True
    except InvalidSignature:
        return False


# ============================================================
# Example Usage
# ============================================================

def main():
    test_file_path = os.path.join(BASE_DIR, "test.txt")

    if not os.path.exists(test_file_path):
        with open(test_file_path, "wb") as file:
            file.write(b"Hello world!! Test Crypto Module!!!")

    private_key_path, public_key_path = rsa_key_generation()

    with open(test_file_path, "rb") as file:
        file_data = file.read()

    print("Plain text:", file_data)

    aes_key = os.urandom(32)
    iv, encrypted_text = encrypt_file_aes(file_data, aes_key)
    decrypted_text = decrypt_file_aes(encrypted_text, aes_key, iv)

    print("AES encrypted:", encrypted_text)
    print("AES decrypted:", decrypted_text)

    file_hash = compute_file_hash(test_file_path)
    print("SHA-256:", file_hash)

    signature = sign_file(file_data, private_key_path)
    print("Signature:", signature)

    if verify_sign(signature, file_data, public_key_path):
        print("Verify success")
    else:
        print("Verify failed")


if __name__ == "__main__":
    main()
