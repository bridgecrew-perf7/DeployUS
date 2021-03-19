CREATE DATABASE deployusdb;
use deployusdb;

CREATE TABLE scripts (
  name TEXT PRIMARY KEY,
  cre_date DATE,
  contents BLOB
);
