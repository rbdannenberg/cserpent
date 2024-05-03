import os
import unittest
import subprocess


SRP_TESTS = 'tests/serpent'


class TestExitCode(unittest.TestCase):
    pass


for root, _, files in os.walk(SRP_TESTS):
    for file in files:
        file_name = os.path.join(root, file)
        test_name = 'test_' + file.split('.')[0]

        def test_case(self):
            result = subprocess.run(
                ["serpent64", file_name],
                capture_output=True
            )

            # hack to get the exit code from the compiler (Serpent does not
            # support non-zero exit code)
            stdout = result.stdout.decode('utf-8').strip()
            lines = stdout.split("\n")
            ret_code = lines[-1]
            
            if not ret_code.isdigit():
                self.fail(f'[{file_name}] Unexpected test stdout: {lines[-1]}')

            if int(ret_code) != 0:
                # using `lines[1]` because `lines[0]` captures the Serpent env
                # output instead of the script output
                self.fail(f"[{file_name}] {lines[1]}")

        setattr(TestExitCode, test_name, test_case)


if __name__ == "__main__":
    unittest.main()
