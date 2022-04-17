#include <chrono>
#include <iostream>
#include <string>
#include <utility>

#include "List.h"
/**
 * @brief функция осуществляет один цикл генерации, сериализации, очистки и
 * десериализации
 *
 * @param count - количество элементов в списке
 */
void makeOneLoop(uint32_t count) {

  auto outFilePath = "./out.blist";
  auto outFileHandler = fopen(outFilePath, "wb");

  List::List myList;
  myList.print();
  myList.generate(count);
  myList.print();
  myList.Serialize(outFileHandler);
  fflush(outFileHandler);
  fclose(outFileHandler);
  myList.clear();
  myList.print();
  auto inFilePath = "./out.blist";
  auto inFileHandler = fopen(inFilePath, "rb");
  myList.Deserialize(inFileHandler);
  myList.print();
  fclose(inFileHandler);
};
/**
 * @brief Get the Current Time Ms object
 *
 * @return int64_t
 */
int64_t getCurrentTimeMs() {
  const auto p0 = std::chrono::time_point<std::chrono::high_resolution_clock>{};
  const auto p3 = std::chrono::high_resolution_clock::now();

  auto tstamp = p3 - p0;
  return std::chrono::duration_cast<std::chrono::milliseconds>(tstamp).count();
}
/**
 * @brief простой профилировщик
 *
 */
class Profiler {
public:
  Profiler(std::string label) {
    constructTime = getCurrentTimeMs();
    this->label = std::move(label);
  }

  ~Profiler() {
    destructTime = getCurrentTimeMs();

    std::cout << label << " take " << destructTime - constructTime << "ms"
              << std::endl;
  }

protected:
  std::string label;
  int64_t constructTime;
  int64_t destructTime{};
};

/**
 * @brief тест функция бинаря.
 *
 * если передать единственным аргументом число - оно станет количеством
 * элементов для одиночного теста, если передать два параметра "perf" и число,
 * то запустится условно-silent тест, который будет последовательно вызывать
 * тест от 0 до количества элементов, указанных в числе и показывать примерное
 * время выполнения
 *
 * @param argc очевидно
 * @param argv очевидно
 * @return int очевидно
 */
int main(int argc, char *argv[]) {
  srand(time(nullptr));
  auto count = 0;
  if (argc == 2) {
    count = std::atoi(argv[1]);
  }
  if (argc == 3) {
    if (std::string(argv[1]) == "perf") {
      List::List::perfTest = true;
    }
    count = std::atoi(argv[2]);
  }
  if (!List::List::perfTest) {
    makeOneLoop(count);
  } else {
    for (auto i = 0; i < count; ++i) {
      Profiler give_me_a_name(std::to_string(i));
      makeOneLoop(i);
    }
  }
  return 1;
}