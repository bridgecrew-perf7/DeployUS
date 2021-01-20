#pragma once

#include "commands/BaseCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/InitCommand.hpp"
#include "commands/AddCommand.hpp"
#include "commands/CommitCommand.hpp"
#include "commands/CheckoutCommand.hpp"

BaseCommand* parse_args(int argc, char* argv[]);
