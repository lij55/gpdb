-- ========
-- PROTOCOL
-- ========

-- create the database functions
CREATE OR REPLACE FUNCTION read_from_pb() RETURNS integer AS
        '$libdir/gppb.so', 'pb_import' LANGUAGE C STABLE;

-- declare the protocol name along with in/out funcs
CREATE PROTOCOL pb (
        readfunc  = read_from_pb
);

drop external table pbexample;
create READABLE external table pbexample (date text, time text, open text, high float) location('pb://localhost/data config=/home/gpadmin/pb.prob column=a,b,c') FORMAT 'csv';

SELECT count(*) FROM pbexample;
-- SELECT sum(open) FROM pbexample;
-- SELECT avg(open) FROM pbexample;

-- =======
-- CLEANUP
-- =======
DROP EXTERNAL TABLE pbexample;
