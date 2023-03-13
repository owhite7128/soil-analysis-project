import sys
import logging

logging.basicConfig (level=logging.DEBUG, filename='/var/www/sql_expose/logs/sql_expose.log', format="%(asctime)s : %(message)s")
sys.path.insert(0,"/var/www/sql_expose")
from app import app as application