#include "standart_library_func.h"

void AddSystemFunction(std::shared_ptr<Scope> global, std::ostream &output) {
  global->Assign("print",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       for (const auto &v : args) {
                         Interpret::Print(v, output);
                       }
                       return nullptr;
                     }});

  global->Assign("println",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       for (const auto &v : args) {
                         Interpret::Print(v, output);
                       }
                       output << std::endl;
                       return nullptr;
                     }});

  global->Assign("abs",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error("No argument for abs");
                       }

                       double res;
                       try {
                         res = std::abs(std::get<double>(args[0]));
                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a number");
                       }

                       return Value(res);
                     }});

  global->Assign("ceil",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error("No argument for ceil");
                       }

                       double res;
                       try {
                         res = std::ceil(std::get<double>(args[0]));
                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a number");
                       }

                       return Value(res);
                     }});

  global->Assign(
      "floor", std::function<Value(const std::vector<Value> &)>{
                   [&output](const std::vector<Value> &args) -> Value {
                     if (args.size() != 1) {
                       throw std::runtime_error("No argument for floor");
                     }

                     double res;
                     try {
                       res = std::floor(std::get<double>(args[0]));
                     } catch (const std::exception &e) {
                       throw std::runtime_error("Argument must be a number");
                     }

                     return Value(res);
                   }});

  global->Assign(
      "round", std::function<Value(const std::vector<Value> &)>{
                   [&output](const std::vector<Value> &args) -> Value {
                     if (args.size() != 1) {
                       throw std::runtime_error("No argument for round");
                     }

                     double res;
                     try {
                       res = std::round(std::get<double>(args[0]));
                     } catch (const std::exception &e) {
                       throw std::runtime_error("Argument must be a number");
                     }

                     return Value(res);
                   }});

  global->Assign("sqrt",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error("No argument for sqrt");
                       }

                       double res;
                       try {
                         res = std::sqrt(std::get<double>(args[0]));
                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a number");
                       }

                       return Value(res);
                     }});

  global->Assign("rnd",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error("No argument for rnd");
                       }

                       double res;
                       try {
                         res = std::get<double>(args[0]);
                         static std::mt19937 gen(std::random_device{}());
                         std::uniform_int_distribution<> dist(0, res - 1);
                         int result = dist(gen);
                         return Value(static_cast<double>(result));

                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a number");
                       }
                     }});

  global->Assign("parse_num",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error(
                             "parse_num needs one argument");
                       }

                       double res;
                       try {
                         res = std::stod(std::get<std::string>(args[0]));

                       } catch (const std::exception &e) {
                         return Value(nullptr);
                       }

                       return Value(res);
                     }});

  global->Assign("to_string",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error(
                             "parse_num needs one argument");
                       }

                       std::string res;
                       try {
                         res = std::to_string(std::get<double>(args[0]));

                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a number");
                       }

                       return Value(res);
                     }});

  global->Assign(
      "len",
      std::function<Value(const std::vector<Value> &)>{
          [&output](const std::vector<Value> &args) -> Value {
            if (args.size() != 1) {
              throw std::runtime_error("len needs one argument");
            }

            double res;

            if (std::holds_alternative<std::string>(args[0])) {
              try {
                res = (std::get<std::string>(args[0])).length();

              } catch (const std::exception &e) {
                throw std::runtime_error("Argument must be a string or a list");
              }

              return Value(res);
            }

            if (std::holds_alternative<std::vector<Value>>(args[0])) {
              try {
                res = (std::get<std::vector<Value>>(args[0])).size();

              } catch (const std::exception &e) {
                throw std::runtime_error(
                    "Argument must be a string or a list");
              }

              return Value(res);
            }

            throw std::runtime_error("Argument must be a string or a list");
          }});

  global->Assign(
      "lower", std::function<Value(const std::vector<Value> &)>{
                   [&output](const std::vector<Value> &args) -> Value {
                     if (args.size() != 1) {
                       throw std::runtime_error("lower needs one argument");
                     }

                     std::string s, res = "";
                     try {
                       s = (std::get<std::string>(args[0]));
                       for (int i = 0; i < s.length(); ++i) {
                         res += std::tolower(s[i]);
                       }

                     } catch (const std::exception &e) {
                       throw std::runtime_error("Argument must be a string");
                     }

                     return Value(res);
                   }});

  global->Assign(
      "upper", std::function<Value(const std::vector<Value> &)>{
                   [&output](const std::vector<Value> &args) -> Value {
                     if (args.size() != 1) {
                       throw std::runtime_error("upper needs one argument");
                     }

                     std::string s, res = "";
                     try {
                       s = (std::get<std::string>(args[0]));
                       for (int i = 0; i < s.length(); ++i) {
                         res += std::toupper(s[i]);
                       }

                     } catch (const std::exception &e) {
                       throw std::runtime_error("Argument must be a string");
                     }

                     return Value(res);
                   }});

  global->Assign(
      "split",
      std::function<Value(const std::vector<Value> &)>{
          [&output](const std::vector<Value> &args) -> Value {
            if (args.size() != 2) {
              throw std::runtime_error("split needs two arguments");
            }

            std::string s, delim;
            std::vector<Value> parts;
            try {
              s = std::get<std::string>(args[0]);
              delim = std::get<std::string>(args[1]);

              size_t pos = 0, next;
              while ((next = s.find(delim, pos)) != std::string::npos) {
                parts.emplace_back(s.substr(pos, next - pos));
                pos = next + delim.size();
              }

              parts.emplace_back(s.substr(pos));

            } catch (const std::exception &e) {
              throw std::runtime_error("Argument must be a string");
            }

            return Value(std::move(parts));
          }});

  global->Assign("join",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 2) {
                         throw std::runtime_error("join needs two arguments");
                       }

                       std::string res = "", delim;
                       std::vector<Value> v;
                       try {
                         v = std::get<std::vector<Value>>(args[0]);
                         delim = std::get<std::string>(args[1]);

                         for (int i = 0; i < v.size(); ++i) {
                           res += std::get<std::string>(v[i]);
                           if (i != v.size() - 1) {
                             res += delim;
                           }
                         }

                       } catch (const std::exception &e) {
                         throw std::runtime_error("Argument must be a string");
                       }

                       return Value(res);
                     }});

  global->Assign("replace",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 3) {
                         throw std::runtime_error(
                             "replace needs three arguments");
                       }

                       std::string res = "", s, old, new_s;
                       try {
                         s = std::get<std::string>(args[0]);
                         old = std::get<std::string>(args[1]);
                         new_s = std::get<std::string>(args[2]);

                         size_t pos = 0;
                         while ((pos = s.find(old, pos)) != std::string::npos) {
                           s.replace(pos, old.length(), new_s);
                           pos += new_s.length();
                         }

                       } catch (const std::exception &e) {
                         throw std::runtime_error(
                             "Argument must be a string or a non-empty list");
                       }

                       return Value(std::move(s));
                     }});

  global->Assign(
      "range", std::function<Value(const std::vector<Value> &)>{
                   [&output](const std::vector<Value> &args) -> Value {
                     if (args.size() != 3) {
                       throw std::runtime_error("range needs three arguments");
                     }

                     double first, last, step;

                     std::vector<Value> res;
                     try {
                       first = std::get<double>(args[0]);
                       last = std::get<double>(args[1]);
                       step = std::get<double>(args[2]);

                       if (step == 0) {
                         throw std::runtime_error("Step can not be 0");
                       }

                       for (double i = first; i < last; i += step) {
                         res.push_back(i);
                       }

                     } catch (const std::exception &e) {
                       throw std::runtime_error(
                           "Range must be numeric");
                     }

                     return Value(std::move(res));
                   }});

  global->Assign("push",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 2) {
                         throw std::runtime_error("push needs two arguments");
                       }

                       std::vector<Value> res;
                       try {
                         res = std::get<std::vector<Value>>(args[0]);
                         res.push_back(args[1]);

                       } catch (const std::exception &e) {
                         throw std::runtime_error(
                             "Argument must be a list");
                       }

                       return Value(std::move(res));
                     }});

  global->Assign("pop",
                 std::function<Value(const std::vector<Value> &)>{
                     [&output](const std::vector<Value> &args) -> Value {
                       if (args.size() != 1) {
                         throw std::runtime_error("pop needs one argument");
                       }

                       std::vector<Value> v;
                       Value res;
                       try {
                         v = std::get<std::vector<Value>>(args[0]);
                         res = v[v.size() - 1];
                         v.pop_back();

                       } catch (const std::exception &e) {
                         throw std::runtime_error(
                             "Argument must be a list");
                       }

                       return res;
                     }});

  global->Assign(
      "insert",
      std::function<Value(const std::vector<Value> &)>{
          [&output](const std::vector<Value> &args) -> Value {
            if (args.size() != 3) {
              throw std::runtime_error("insert needs three arguments");
            }

            std::vector<Value> v;
            double index;
            Value value;

            try {
              v = std::get<std::vector<Value>>(args[0]);
              index = std::get<double>(args[1]);
              value = args[2];
              v.insert(v.begin() + index, value);

            } catch (const std::exception &e) {
              throw std::runtime_error(
                  "Index must be integer");
            }

            return Value(v);
          }});

  global->Assign(
      "remove",
      std::function<Value(const std::vector<Value> &)>{
          [&output](const std::vector<Value> &args) -> Value {
            if (args.size() != 2) {
              throw std::runtime_error("remove needs two arguments");
            }

            std::vector<Value> v;
            double index;

            try {
              v = std::get<std::vector<Value>>(args[0]);
              index = std::get<double>(args[1]);
              v.erase(v.begin() + index);

            } catch (const std::exception &e) {
              throw std::runtime_error(
                  "Index must be integer");
            }

            return Value(v);
          }});

  global->Assign(
      "sort",
      std::function<Value(const std::vector<Value> &)>{
          [&output](const std::vector<Value> &args) -> Value {
            if (args.size() != 1) {
              throw std::runtime_error("sort needs one argument");
            }

            std::vector<Value> v = std::get<std::vector<Value>>(args[0]);

            try {
              auto alt = v[0].index();
              for (auto &el : v) {
                if (el.index() != alt) {
                  throw std::runtime_error(
                      "All elements must be of one type");
                }
              }

              auto cmp = [&](const Value &a, const Value &b) {
                switch (alt) {
                case 0:
                  return std::get<double>(a) < std::get<double>(b);
                case 1:
                  return std::get<std::string>(a) < std::get<std::string>(b);
                case 2:
                  return std::get<bool>(a) < std::get<bool>(b);
                default:
                  throw std::runtime_error(
                      "Unlnown type for sort");
                }
              };

              std::sort(v.begin(), v.end(), cmp);
              return Value(std::move(v));
            } catch (std::exception &e) {
              throw std::runtime_error(
                  "All elements must be of one type");
            }
          }});

  global->Assign("read", std::function<Value(const std::vector<Value> &)>{
                             [](auto &args) -> Value {
                               std::string line;
                               if (!std::getline(std::cin, line))
                                 return nullptr;
                               return Value(line);
                             }});

  global->Assign("stacktrace",
                 std::function<Value(const std::vector<Value> &)>{
                     [](const std::vector<Value> &) -> Value {
                       std::vector<Value> out;
                       for (const auto &frame : Interpret::GetStackTrace()) {
                         out.emplace_back(frame);
                       }
                       return Value::MakeList(std::move(out));
                     }});
}
