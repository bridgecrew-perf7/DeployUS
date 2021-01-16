#pragma once

#include "commands/BaseCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/InitCommand.hpp"

BaseCommand* parse_args(int argc, char* argv[]);
