CREATE DATABASE deployusdb;
use deployusdb;

CREATE TABLE scripts (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name TEXT,
  cre_date DATE,
  contents BLOB
);
