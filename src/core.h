/** @file @brief Вычисления, хеширование SHA-1 и база аутентификации. */
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
namespace course {
/** @brief Вычисляет SHA-1 и возвращает 40 заглавных шестнадцатеричных символов.
 * @param text Входная последовательность байтов.
 * @return Шестнадцатеричное представление SHA-1.
 */
std::string sha1(const std::string& text);
/** @brief Загружает пары login:password из текстового файла при старте.
 * @param path Путь к базе клиентов.
 * @return Словарь логинов и паролей.
 * @throw std::runtime_error При отсутствии файла, пустой базе или неверной записи.
 */
std::map<std::string,std::string> load_users(const std::string& path);
/** @brief Проверяет LOGIN || SALT16 || HASH40 с SHA1(SALT16 || PASSWORD).
 * @param message Текстовое сообщение аутентификации без разделителей.
 * @param users База пользователей.
 * @return true для корректного сообщения, false иначе.
 */
bool authenticate(const std::string& message, const std::map<std::string,std::string>& users);
/** @brief Сумма квадратов беззнаковых 16-битных значений с насыщением 65535.
 * @param values Элементы вектора.
 * @return Сумма квадратов либо 65535 при переполнении вверх.
 */
uint16_t sum_squares(const std::vector<uint16_t>& values);
}
