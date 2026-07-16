I’m going to lay this out as a build roadmap from “empty repo” to a real mini-DB, with each phase ending in something usable and explainable.

Absolutely. For a first solo project, the best way is to build it in layers, where each layer leaves you with something working and understandable.

I would strongly recommend this stack:

Core stack

C++20
CMake
GoogleTest for tests
clang-format for formatting
Git + GitHub for version control

Nice-to-have

spdlog for logging
fmt for cleaner string formatting

Optional later

A tiny CLI/REPL interface
A simple benchmark script in Python if you want performance graphs

No web framework, no frontend, no fancy stack. For a DBMS project, the whole point is the systems code.

Big picture

Do not start by trying to build “a whole database.”

Start by building a tiny SQL engine that gradually becomes a real DBMS.

The core idea is:

accept commands,
parse them,
store data,
query data,
persist data,
optimize data access,
handle concurrency and recovery if you go far enough.
Phase 0: Project setup and design
Problem statement

Before writing database code, decide what the database is supposed to do at a minimum.

What you do

Create a design doc with:

supported commands
data types
storage format
file layout
whether you use pages
whether you support only one table at first

Set up:

repository
CMake build
test framework
directory structure
At the end of this phase, your DBMS should be able to

Nothing yet, but your project should be ready to build cleanly.

Suggested scope

Start with this tiny SQL subset:

CREATE TABLE users(id INT, name TEXT);
INSERT INTO users VALUES (1, 'Abhishek');
SELECT \* FROM users;
SELECT name FROM users WHERE id = 1;
DELETE FROM users WHERE id = 1;

That is enough for a strong project base.

Suggested folder structure
dbms/
├── src/
│ ├── parser/
│ ├── execution/
│ ├── storage/
│ ├── index/
│ ├── transaction/
│ ├── recovery/
│ └── common/
├── include/
├── tests/
├── docs/
└── CMakeLists.txt
Phase 1: Interactive shell and command handling
Problem statement

How does the user talk to your database?

You need a way to type commands and get responses.

What you do

Build a simple REPL:

db> CREATE TABLE users(id INT, name TEXT);
db> INSERT INTO users VALUES (1, 'Abhishek');
db> SELECT \* FROM users;

At this stage, you do not need full SQL correctness. Just make the shell accept input and route it to the right part of the program.

End goal

Your DBMS should:

start from terminal
accept one command at a time
print results or errors
What you learn
program structure
input handling
basic command dispatch
Phase 2: Tokenizer and parser
Problem statement

The DBMS must understand text like:

SELECT name FROM users WHERE id = 1;

That means turning raw text into a structured internal representation.

What you do

Build:

a tokenizer: breaks input into tokens like SELECT, name, FROM, users, WHERE, id, =, 1
a parser: turns tokens into an AST or command object

For example:

SelectQuery {
table = "users",
columns = {"name"},
where = id = 1
}
End goal

Your DBMS should:

parse supported commands
reject invalid syntax cleanly
convert SQL strings into internal objects
Keep it simple

At first, only support:

CREATE TABLE
INSERT
SELECT
DELETE

No joins. No subqueries. No fancy expressions.

Why this matters

This is the compiler part of the database. It is the gateway to everything else.

Phase 3: In-memory table storage
Problem statement

Once a command is parsed, where does the data live?

What you do

Implement a simple in-memory representation for:

tables
rows
columns
values

A basic approach:

each table has a schema
each table stores a vector of rows
each row stores a vector or struct of values

Example:

struct Row {
vector<Value> values;
};
End goal

Your DBMS should be able to:

create a table in memory
insert rows
scan rows
print selected rows
delete rows
Important

At this stage, do not worry about disk yet.

First make correctness easy.

What to watch for
type handling: int, string, maybe bool
null handling, if you want it
schema validation on insert
duplicate column names
Phase 4: Basic execution engine
Problem statement

You now have parsed commands and in-memory data. How do you execute a query?

What you do

Write an execution layer that performs:

table creation
row insertion
full table scan
filtering with WHERE
projection with SELECT col1, col2

Think of execution as:

take AST
run operations on storage
return result
End goal

Your DBMS should support:

INSERT INTO users VALUES (1, 'A');
SELECT \* FROM users;
SELECT name FROM users WHERE id = 1;
DELETE FROM users WHERE id = 1;
Why this phase matters

This is where the database becomes real.

You now have a working engine, even though it is still simple.

Phase 5: File persistence, version 1
Problem statement

If the program exits, all data is lost. That is not a database yet.

What you do

Add persistent storage.

Start simple:

write the full table to disk
load it back on startup

You can use:

a single file per table, or
one database file for everything

At first, a simple serialization format is fine.

End goal

Your DBMS should:

save data to disk
reload data after restart
preserve tables and rows across sessions
What the DBMS can do now
create table
insert rows
shut down
restart
still have the data
Keep the format simple

You do not need WAL yet.
You do not need pages yet.
You do not need recovery yet.

Just make data survive a restart.

Phase 6: Page-based storage
Problem statement

Storing the whole table as one blob is easy, but real databases use pages because large data needs structured I/O.

What you do

Introduce:

fixed-size pages, for example 4 KB
page IDs
page reading and writing
page headers if needed

Now your storage engine should work with pages rather than one giant file dump.

End goal

Your DBMS should:

allocate pages
store rows inside pages
read/write specific pages
avoid rewriting the entire database file for every change
Why this is important

This is the step where the project starts feeling like a real DBMS instead of a toy.

Phase 7: Buffer pool
Problem statement

Reading from disk for every operation is slow. Databases keep hot pages in memory.

What you do

Build a buffer pool manager:

loads pages from disk into memory
keeps a limited number of pages cached
evicts pages when full

A simple replacement policy:

LRU
or FIFO if you want something easier first
End goal

Your DBMS should:

cache pages in memory
reduce disk reads/writes
fetch pages through the buffer manager instead of directly from disk
What you learn

This is one of the most valuable OS/database concepts:

caching
eviction
dirty pages
pinned pages
Important terms
pin: page is in use, cannot be evicted
dirty: page changed in memory and not yet written back
unpin: page is done being used

Phase 8: Indexing with a B+ tree
Problem statement

A full table scan is slow. You need fast lookups.

What you do

Implement indexing.

A good route:

start with a simple hash index for equality lookups
then build a B+ tree for ordered keys and range scans
End goal

Your DBMS should support:

CREATE INDEX
fast WHERE id = x
possibly range queries like WHERE id BETWEEN 10 AND 20
What the DBMS should do at the end

Instead of scanning all rows, it should:

find the key in the index
jump to the row quickly
Why B+ trees

Yes, B+ trees are a data structure, but in databases they are special because they are:

shallow
disk-friendly
good for range scans
good for ordered traversal
What you do not need at first

You do not need to master every edge case immediately.
You should understand:

node structure
insertion
split
merge
leaf chaining
why fanout matters
Phase 9: Query execution using indexes
Problem statement

The database should be smart enough to use the index when it helps.

What you do

Add a simple planner:

if WHERE matches an indexed key, use index lookup
otherwise do a table scan
End goal

Your DBMS should be able to:

decide between scan and index
execute indexed queries
return the same result either way
Example

For this query:

SELECT \* FROM users WHERE id = 10;

the engine should ideally:

detect id is indexed
use the B+ tree
fetch the row directly

This is where your project becomes much more impressive to explain.

Phase 10: Constraints and schema enforcement
Problem statement

The database should not accept invalid data.

What you do

Add:

column types
NOT NULL
PRIMARY KEY or uniqueness
basic validation on INSERT and UPDATE
End goal

Your DBMS should reject invalid rows and enforce schema rules.

Example behavior

If schema is:

CREATE TABLE users(id INT, name TEXT);

then this should fail:

INSERT INTO users VALUES ('wrong', 123);
Phase 11: Transactions
Problem statement

What happens if multiple operations should succeed or fail together?

What you do

Implement transactions:

BEGIN
COMMIT
ROLLBACK

Start with a simple single-user transaction model.

End goal

Your DBMS should:

group operations
commit them atomically
undo them on rollback
Simple implementation idea

Keep a transaction log in memory first.
Later connect it to persistence.

Why this is important

This is one of the defining features of DBMS software.

Phase 12: Concurrency control
Problem statement

Multiple clients may access the database at the same time.

What you do

Add thread safety:

mutexes
reader-writer locks
lock manager if you go deeper

Start with:

one global lock if needed
Then improve to:
per-table lock
page-level lock
row-level lock if ambitious
End goal

Your DBMS should:

allow multiple concurrent readers
prevent unsafe concurrent writes
avoid corrupted state
What to learn

This is where your OS interest pays off:

race conditions
deadlocks
lock ordering
isolation
Phase 13: Write-ahead logging and recovery
Problem statement

If the process crashes, how do you restore consistency?

What you do

Add WAL:

before changing data, write the change to a log file
after crash, replay the log

Then add recovery:

load data
replay committed operations
ignore incomplete ones
End goal

Your DBMS should:

survive crashes
recover from log
restore a usable state
Why this is advanced

This is one of the most important real database concepts.

If you reach this phase, your project becomes very serious.

Phase 14: Query optimization
Problem statement

There may be multiple ways to execute a query. Which is best?

What you do

Add a simple optimizer:

estimate whether to scan or use index
maybe estimate cost using row count
choose a plan
End goal

Your DBMS should:

generate execution plans
choose a better plan when possible
Keep it simple

A real optimizer is huge.
For your project, a heuristic-based optimizer is enough.

Phase 15: Testing, benchmarks, and documentation
Problem statement

A DBMS is only useful if it is reliable and explainable.

What you do

Write:

unit tests
integration tests
crash tests
query tests
performance benchmarks

Also write documentation:

architecture overview
command list
storage format
index design
transaction model
End goal

Your DBMS should be:

testable
understandable
demonstrable
resume-ready
This part matters a lot

A clean README and diagrams can make a huge difference.

A good build order for you

If this is your first solo project, I would build in this order:

REPL
parser
in-memory tables
basic query execution
disk persistence
pages
buffer pool
index
transactions
concurrency
WAL/recovery
optimizer
tests/docs

That order keeps you from getting lost.

What I would actually recommend as your first full version

Your Version 1 should be:

REPL
parser
CREATE TABLE
INSERT
SELECT
DELETE
in-memory storage
simple persistence
one index type
tests

That is already enough to be proud of.

Then version 2 can add:

B+ tree
buffer pool
transactions
concurrency

Then version 3 can add:

WAL
crash recovery
optimizer
What each finished stage gives you

Here is the progression in one line each:

Phase 1–2: database can understand commands
Phase 3–4: database can store and query data in memory
Phase 5: data survives restart
Phase 6–7: storage becomes page-based and efficient
Phase 8–9: queries become fast with indexing
Phase 10: data is validated
Phase 11–12: multiple operations and clients can work safely
Phase 13: system can recover from crashes
Phase 14: database can choose better plans
Phase 15: project becomes polished and presentable
One very important note

Do not try to do all advanced features at once.

The project should always be in a state where:

it compiles
tests pass
README is updated
the current version is usable

That is how solo projects stay manageable.

My recommendation for how to begin tomorrow

Start with exactly this:

create the repo
set up CMake and GoogleTest
write the REPL
implement a tokenizer for CREATE TABLE, INSERT, SELECT
make the parser return structured command objects
run the first command end to end

That first end-to-end path is the most important milestone.

If you want, I can turn this into a
week-by-week implementation plan and also give you a
directory structure + class design for the first phase in C++.
