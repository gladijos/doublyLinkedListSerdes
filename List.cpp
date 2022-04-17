#include "List.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace List {

bool List::perfTest = false;

bool serInt(FILE *file, const int32_t &data) {
  int putInt;
  auto *pData = (char *)(&(data));
  for (auto i = 0; i < sizeof(int32_t) / sizeof(char); ++i) {
    putInt = 0;
    memcpy(&putInt, pData + i, 1);
    if (putInt != fputc(putInt, file)) {
      std::cout << __func__ << " FAIL on ser of int " << data << std::endl;
      return false;
    }
  }
  return true;
}
bool desInt(FILE *file, int32_t &data) {
  int getInt;
  auto *pData = (char *)(&(data));
  for (auto i = 0; i < sizeof(int32_t) / sizeof(char); ++i) {
    getInt = fgetc(file);
    if (getInt == EOF) {
      std::cout << __func__ << " FAIL on deser of int" << std::endl;
      return false;
    }
    pData[i] = (char)getInt;
  }
  return true;
}
bool serData(FILE *file, const std::string &data) {
  int32_t length = data.length();
  bool res = serInt(file, length);
  if (!res) {
    return false;
  }

  for (auto i = 0; i < length; ++i) {
    int charInt{0};
    memcpy(&charInt, &data[i], 1);
    if (charInt != fputc(charInt, file)) {
      std::cout << __func__ << " FAIL on ser of character " << charInt
                << std::endl;
      return false;
    }
  }

  return true;
}
bool desData(FILE *file, std::string &data) {
  int32_t length{};
  bool res = desInt(file, length);
  if (!res) {
    return false;
  }
  data.resize(length);
  int getInt;

  for (auto i = 0; i < length; ++i) {
    getInt = fgetc(file);
    if (getInt == EOF) {
      std::cout << __func__ << " FAIL on deser of int" << std::endl;
      return false;
    }
    data[i] = (char)getInt;
  }
  return true;
}

void List::Serialize(FILE *file) {
  if (!serInt(file, count)) {
    return;
  }
  helper.serCache.resize(count);
  {

    ListNode *cur = head;
    for (auto &item : helper.serCache) {
      if (cur == nullptr) {
        std::cout << __func__ << " FAIL on ser cur is null" << std::endl;
      }
      item = cur;
      cur = cur->next;
    }
    for (auto &item : helper.serCache) {
      int32_t randIdx = -1;
      if (item->rand != nullptr) {

        auto randIt = std::find(helper.serCache.begin(), helper.serCache.end(),
                                item->rand);
        if (randIt != helper.serCache.end()) {
          randIdx = std::distance(helper.serCache.begin(), randIt);
        }
      }

      if (!(serData(file, item->data) && serInt(file, randIdx))) {
        return;
      }
    }
  }
}
bool List::readItem(ListNode &newList, FILE *file) {
  int32_t randIdx;
  if (!(desData(file, newList.data) && desInt(file, randIdx))) {
    return false;
  }
  if (randIdx == -1) {
    newList.rand = nullptr;
  } else {
    newList.rand = &(helper.deserCache[randIdx]);
  }
  return true;
}

void List::Deserialize(FILE *file) {
  auto count{0};
  if (!desInt(file, count)) {
    return;
  }
  this->count = count;
  helper.deserCache.resize(count);

  ListNode *prev = nullptr;
  ListNode *cur = nullptr;
  ListNode *next = nullptr;
  head = nullptr;
  tail = nullptr;

  if (count > 0) {
    head = cur = &(helper.deserCache[0]);
    if (count > 1) {
      next = cur + 1;
    }
    cur->prev = prev;
    cur->next = next;
    if (!readItem(*cur, file)) {
      return;
    }
    prev = cur;
    cur = next;
    ++next;
    for (auto i = 1; i < count - 1; ++i, ++cur, ++prev, ++next) {
      cur->prev = prev;
      cur->next = next;
      if (!readItem(*cur, file)) {
        return;
      }
    }
    if (count > 1) {
      cur->prev = prev;
      cur->next = nullptr;
      if (!readItem(*cur, file)) {
        return;
      }
    }
  }
  tail = cur;
}

std::string genRandom(const int len) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}

ListNode *createNode() {
  auto newNode = new (ListNode);
  auto len = std::rand() / 200000000;
  newNode->data = genRandom(len);
  newNode->rand = nullptr;
  return newNode;
}

ListNode *List::findByIdx(int32_t idx) {
  auto *result = head;
  for (auto i = 0; i < idx; ++i) {
    if ((result != nullptr) && (result->next != nullptr)) {
      result = result->next;
    } else {
      result = nullptr;
      break;
    }
  }
  return result;
}

int32_t List::getIdx(ListNode *node) {
  if (node == nullptr) {
    return -1;
  }
  uint32_t result = 0;
  auto *cur = node;
  while (cur != head) {
    ++result;
    if (cur == nullptr) {
      std::cout << __func__ << "  unexpected cur is nullptr" << std::endl;
      return -1;
    }
    cur = cur->prev;
  }
  return result;
}

void List::print() {

  if (perfTest) {
    return;
  }
  std::cout << __func__ << " List" << std::endl;
  if (count == 0) {
    std::cout << "List is empty" << std::endl;
  }

  auto *cur = head;
  for (auto i = 0; i < count; ++i) {
    if (cur == nullptr) {
      break;
    }
    std::cout << "IDX " << i << ": cur " << (void *)cur << " prev "
              << (void *)cur->prev << " next " << (void *)cur->next << " rand "
              << (void *)cur->rand << " (idx " << getIdx(cur->rand)
              << ") data: " << std::quoted(cur->data) << std::endl;
    cur = cur->next;
  }
}

void List::generate(uint32_t count) {
  if (!perfTest) {
    std::cout << __func__ << std::endl;
  }
  ListNode *prev = nullptr;
  ListNode *cur = nullptr;

  cur = head = createNode();
  cur->prev = prev;
  prev = cur;
  for (auto i = 1; i < count; ++i) {
    cur = createNode();
    cur->prev = prev;
    prev->next = cur;
    prev = cur;
    auto randIdx = std::rand() * i / 20000000;
    if (randIdx <= i) {
      auto *referringNode = findByIdx(randIdx);
      if (referringNode != nullptr) {
        referringNode->rand = cur;
      }
    }
  }
  tail = cur;
  cur->next = nullptr;
  this->count = count;
}
void List::clear() {
  if (!perfTest) {
    std::cout << __func__ << std::endl;
  }
  auto *cur = head;

  while (cur != nullptr) {
    auto *tmp = cur;
    cur = cur->next;
    delete tmp;
  }
  count = 0;
  head = nullptr;
  tail = nullptr;
}
} // namespace List