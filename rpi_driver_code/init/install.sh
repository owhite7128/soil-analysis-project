#!\bin\sh

# Update 
sudo apt update
echo "Updated\n"

# Install apache2 and mod-wsgi
sudo apt install -y apache2
sudo service apache2 start
sudo apt install -y libapache2-mod-wsgi-py3
echo "Installed Apache2 and mod-wsgi\n"

# Installing MySQL and Initialize Database and Table
sudo apt install -y mysql-server
mysql -u root < mysql_schema.sql
echo "Installed MySQL and Created Table\n"

# Copy to /var/www/sql_expose and Move to it
sudo cp -R ../ /var/www/sql_expose
cd /var/www/sql_expose
echo "Copied to /var\n"

# Install Required Python Modules
python3 -m pip install flask
python3 -m pip install flask_mysqldb
echo "Installed Python Modules\n"

# Create Logfile and Change Perms
sudo touch ./logs/sql_expose.log
sudo chgrp www-data ./logs/sql_expose.log
sudo chmod 464 ./logs/sql_expose.log
echo "Created Logfile\n"

# Configuring Apache2 to use WSGI
cd /etc/apache2/sites-available
sudo touch sql_expose.conf
echo '<VirtualHost *:80>\n\tServerName: sql_expose.com\n\tServerAlias: sql_expose.com\n\tWSGIDaemonProcess sql_expose threads=5\n\tWSGIScriptAlias / /var/www/sql_expose/sql_expose.wsgi\n\t<Directory /var/www/sql_expose>\n\t\tWSGIProcessGroup sql_expose\n\t\tWSGIApplicationGroup %{GLOBAL}\n\t\tRequire all granted\n\t</Directory>\n</VirtualHost>' > sql_expose.conf
echo "Configured Apache2 File for WSGI\n"

# Enable Site
sudo a2ensite sql_expose.conf
sudo a2dissite 000-default.conf
echo "Enabled sql_expose and Disabled 000-default.conf"

# Restart Apache2
sudo service apache2 restart
echo "Restarted Apache2"