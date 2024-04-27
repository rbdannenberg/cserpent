import os
import unittest
import subprocess


SRP_TESTS = 'tests/serpent'


class TestExitCode(unittest.TestCase):
    pass


for root, _, files in os.walk(SRP_TESTS):
    for file in files:
        file_name = os.path.join(root, file)
        subprocess.run(["serpent64", file_name])
