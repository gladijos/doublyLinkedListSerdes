#pragma once

#include <algorithm>
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

namespace List {

// структуру ListNode модифицировать нельзя
struct ListNode {
  ListNode *prev;
  ListNode *next;
  ListNode *rand; // указатель на произвольный элемент данного списка, либо NULL
  std::string data;
};
/**
 * @brief структура кэшей
 *
 */
struct ListIoHelper {
  std::vector<ListNode *> serCache;
  std::vector<ListNode> deserCache;
};

class List {
public:
  List() = default;
  // деструктор не продуман. clear нельзя вызывать, потому что при
  // десериализации данные создаются не в куче
  ~List() = default;
  // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
  void Serialize(FILE *file);
  // загрузка из файла (файл открыт с помощью  fopen(path, "rb"))
  void Deserialize(FILE *file);
  /**
   * @brief вывод списка в консоль
   *
   */
  void print();
  /**
   * @brief создание и случайное заполнение списка
   *
   * @param count - количество элементов
   */
  void generate(uint32_t count);
  /**
   * @brief очистка списка
   *
   */
  void clear();
  /**
   * @brief Get the Idx object
   *
   * @param node - указатель на объект списка
   * @return int32_t -найденный индекс, иначе "-1"
   */
  int32_t getIdx(ListNode *node);
  /**
   * @brief чтение элемента из файла
   *
   * @param newList - результат
   * @param file - файл
   * @return true при успехе
   * @return false если что-то идёт не так
   */
  bool readItem(ListNode &newList, FILE *file);
  /**
   * @brief поиск по индексу (без кэшей)
   *
   * @param idx - индекс
   * @return ListNode* найденный (или нет) элемент
   */
  ListNode *findByIdx(int32_t idx);
  /**
   * @brief статическое поле , по которому скрывается консольный вывод
   *
   */
  static bool perfTest;

private:
  ListNode *head{nullptr};
  ListNode *tail{nullptr};
  int count{0};
  ListIoHelper helper;
};
} // namespace List