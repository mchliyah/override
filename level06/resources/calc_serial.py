#!/usr/bin/env python
def calculate_serial(login):

    login = login.strip()

    if len(login) <= 5:
        print("Login must be > 5 characters")
        return None

    hash_val = (ord(login[3]) ^ 0x1337) + 0x5eeded

    for char in login:
        ascii_val = ord(char)

        if ascii_val <= 0x1f:
            print("All characters must be printable")
            return None

        xored = ascii_val ^ hash_val
        hash_val += xored % 1337

    return hash_val & 0xFFFFFFFF

login = "mchliyah"
serial = calculate_serial(login)
print("\nLogin: {}".format(login))
print("Serial: {}".format(serial))
