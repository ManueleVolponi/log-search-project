from dataclasses import dataclass
from enum import Enum

class APIMethods(Enum):
    GET='GET'
    POST='POST'
    PUT='PUT'
    PATCH='PATCH'
    DELETE='DELETE'

@dataclass(frozen=True)
class Log:
    date: str
    ip: str
    method: APIMethods
    endpoint: str
    http_version: str
    status_code: int
    ms: int
    
    