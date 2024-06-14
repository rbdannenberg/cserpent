def is_identifier_start (char):
    return char.isalpha() or char == '_'

def is_identifier(char):
    return char.isalnum() or char == '_'
