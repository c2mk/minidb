What each debug executable should test

parser_debug

Tokenizer
Parser
AST generation
No storage/executor

storage_debug

Schema creation
Row insertion
Deletion
TableHeap behavior

executor_debug

Create table
Insert
Select
Delete
Directly construct AST nodes (don't involve parser)

integration_debug

Full SQL string
Tokenizer → Parser → Executor
Mimics real user flow

"preLaunchTask": "CMake: build"
Edit code -> F5 -> Cmake rebuilds changed files -> Debugger Launches

Haven't committed launch.json, so rewrite them before using F5.
