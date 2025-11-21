import unittest
from generate_log import Generator
from pathlib import Path
import os
from faker import Faker
from dotenv import load_dotenv

load_dotenv(dotenv_path=f"{Path(__file__).resolve().parent.parent}/.env.test", override=True)

class GeneratorTest(unittest.TestCase):
    def setUp(self):
        self.generator = Generator(Faker())
        self.root_path = Path(__file__).resolve().parent.parent.parent
        self.filename = os.getenv("LOG_FILENAME")
        self.full_filepath = self.root_path / self.filename
        
    def test_generate_logs(self):
        self.generator.log_generator(self.full_filepath)
        self.assertTrue(self.full_filepath.exists())
    
    def tearDown(self):
        self.generator = None
        
        if os.path.exists(self.full_filepath):
            os.remove(self.full_filepath)
            
        self.root_path = None
    
if __name__ == '__main__':
    unittest.main()