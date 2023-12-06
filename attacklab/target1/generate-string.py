def generate_string(content, length):
    result = content * length
    with open('output.txt', 'w') as file:
        file.write(result)


if __name__ == "__main__":
    generate_string("00 ", 40)