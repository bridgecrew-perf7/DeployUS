CREATE DATABASE deployusdb;
use deployusdb;

CREATE TABLE scripts (
  name TEXT,
  path TEXT,
  cre_date DATE
);

INSERT INTO scripts
  (name, path, cre_date)
VALUES
  ('script1', './work','2021-02-02'),
  ('script2', './work2','2022-03-03');