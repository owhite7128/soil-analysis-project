CREATE DATABASE IF NOT EXISTS `soil-analysis` CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
USE `soil-analysis`;
CREATE TABLE IF NOT EXISTS `soil-scans` (
    id int NOT NULL AUTO_INCREMENT,
    datestr varchar(255) NOT NULL,
    img_src varchar(255) NOT NULL,
    quad varchar(255) NOT NULL,
    phdata varchar(1000) NOT NULL,
    PRIMARY KEY(id));