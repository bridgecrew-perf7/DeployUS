CREATE DATABASE deployusdb;
use deployusdb;

CREATE TABLE scripts (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name TEXT NOT NULL,
  cre_date DATETIME NOT NULL,
  contents BLOB NOT NULL
);

CREATE TABLE workers (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name TEXT NOT NULL,
  location TEXT NOT NULL
);

CREATE TABLE jobs (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  script_id INT NOT NULL,
  worker_id INT NOT NULL,
  FOREIGN KEY (script_id) REFERENCES scripts(id),
  FOREIGN KEY (worker_id) REFERENCES workers(id),
  UNIQUE (script_id, worker_id)
);


-- Adding localhost for Part0
INSERT INTO workers (name, location) VALUES
('Localhost','127.0.0.1');