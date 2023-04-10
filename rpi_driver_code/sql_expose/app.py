import json
import flask
from flask import request, jsonify
from flask_mysqldb import MySQL
import MySQLdb.cursors
from datetime import date

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

@app.route('/api/v1/request-scan', methods=["GET", "POST"])
def request_fn():
    if request.method == "POST":
        cursor = mysql.connection.cursor(MySQLdb.cursors.DictCursor)
        json_t = request.json
        if "scan_boundaries" not in json_t:
            return 'bad request', 400
        if "scan_img_name" not in json_t:
            return 'bad request', 400

        if len(json_t["scan_boundaries"]) != 4:
            return 'bad request', 400

        for coord in json_t["scan_boundaries"]:
            if len(coord) != 2:
                return 'bad request', 400

        img_src = json_t["scan_img_name"]

        boundaries = "{} {} {} {} {} {} {} {}".format(json_t["scan_boundaries"][0][0], json_t["scan_boundaries"][0][1], json_t["scan_boundaries"][1][0], json_t["scan_boundaries"][1][1], json_t["scan_boundaries"][2][0], json_t["scan_boundaries"][2][1], json_t["scan_boundaries"][3][0], json_t["scan_boundaries"][3][1])

        cdate = date.today().strftime("%m_%d_%y")
        print(cdate)

        statement = f"INSERT INTO `soil-scans` (`datestr`, `img_src`, `quad`, `phdata`) VALUES (\"{cdate}\", \"{img_src}\", \"{boundaries}\", \"NULL\")"
        print (statement)
        cursor.execute(statement)
        mysql.connection.commit()

        return 'Success', 200
    else:
        return 'bad request', 400


if __name__ == "__main__":
    app.run()