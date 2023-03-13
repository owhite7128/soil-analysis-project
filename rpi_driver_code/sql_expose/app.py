import flask
from flask import request, jsonify
from flask_mysqldb import MySQL
import MySQLdb.cursors

app = flask.Flask(__name__)

app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_DB'] = 'soil-analysis'
app.config['MYSQL_HOST'] = 'localhost'

mysql = MySQL(app)

@app.route('/api/v1/scans', methods=["GET"])
def scans():
    cursor = mysql.connection.cursor(MySQLdb.cursors.DictCursor)
    cursor.execute('SELECT * FROM `soil-scans`')
    scans = cursor.fetchall()
    print (scans)
    scan_dict = {"scans":[scan for scan in scans]}
    return scan_dict


if __name__ == "__main__":
    app.run()