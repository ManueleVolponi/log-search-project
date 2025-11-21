from pathlib import Path
import os
from faker import Faker
from faker.providers import internet
from random import randint, choices
from typing import List
from logs import Log
from dotenv import load_dotenv

load_dotenv(dotenv_path=f"{Path(__file__).resolve().parent}/.env")

class Generator:
    
    def __init__(self, faker: Faker):
        self.fake = faker
        self.fake.add_provider(internet)
        self.logs: List[Log] = []
        self.methods = ['GET', 'POST', 'PUT', 'PATCH', 'DELETE']
        self.weights = [70, 20, 5, 3, 2]
        
    def log_generator(self, filelog_name=None):
        for _ in range(15000):
            new_log_date = self.fake.date_time().strftime('[%d/%b/%Y:%H:%M:%S +0000]')
            new_log_ip = self.fake.ipv4_private()
            
            new_log_method = choices(self.methods, weights=self.weights, k=1)[0]
            
            new_log_endpoint = self.fake.uri_path()
            new_log_http_version = "HTTP/1.1"
            new_log_status_code = self.fake.http_status_code()
            new_log_ms = randint(4, 1500)
            
            log_line = (
                f"{new_log_date},{new_log_ip},{new_log_method},"
                f"{new_log_endpoint},{new_log_http_version},{new_log_status_code},"
                f"{new_log_ms}\n"
            )
            
            self.logs.append(log_line)
            
        self.create_log_file(filename=filelog_name)
            
    def create_log_file(self, filename=None):
        if filename is None:
            current_path = Path(__file__).resolve()
            parent_path = current_path.parent.parent 
            
            final_filepath = parent_path / os.getenv("LOG_FILENAME")
        else:
            final_filepath = filename
            
        if not os.path.exists(final_filepath):
            with open(final_filepath, 'w') as f:
                for log in self.logs:
                    f.write(log)


if __name__ == '__main__':
    fake = Faker()
    gen = Generator(fake)
    gen.log_generator()