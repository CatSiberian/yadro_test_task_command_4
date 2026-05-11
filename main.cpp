#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>

using namespace std;
namespace fs = std::filesystem;

void write_json(string path) {
    const char* home = getenv("HOME");
    if (!home) return;
    string save_path = string(home) + "/scan_results.json";

    while (true) {
        int interval = 3;
        map<string, vector<string>> categories = {{"audio", {}}, {"video", {}}, {"images", {}}};
        /* можно заменить fs::direcroty_iterator() на fs::recrusive_directory_operator()
           и сканировать подпапки, но возникали ошибки, которые пока не поправил */
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_regular_file()) {

                    string ext = entry.path().extension().string();// получаем расширение
                    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);//перевод в нижний регистр

                    string category = "";
                    if (ext == ".mp3" || ext == ".wav" || ext == ".opus") category = "audio";
                    else if (ext == ".mpg" || ext == ".mp4") category = "video";
                    else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".webp") category = "images";

                    if (!category.empty()) {
                        categories[category].push_back(entry.path().filename().string());
                    }
                }
            }

            // Запись JSON
            ofstream file(save_path);
            file << "{\n";
            for (auto it = categories.begin(); it != categories.end(); ++it) {
                file << "  \"" << it->first << "\": [ ";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    file << "\"" << it->second[i] << "\"" << (i == it->second.size() - 1 ? "" : ", ");
                }
                file << " ]" << (next(it) == categories.end() ? "" : ",") << "\n";
            }
            file << "}";
            file.close();

            cout << "Файл обновлен. Ожидание..." << endl;
        } catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
            break;
        }

        this_thread::sleep_for(chrono::seconds(interval));
    }
}

void search_directory() {
    string path;

    cout << " Введите путь к директории, или оставьте пустым, \n если хотите считать данные из каталога /home/\"имя пользователя\" " << endl;
    getline(cin, path);
    while (true) {
        if (fs::exists(path) && fs::is_directory(path)) {
            write_json(path);
            break;
        }
        else if (path.empty()) {
            const char* home = getenv("HOME");
            if (home != nullptr) {
                path = home;
                write_json(path);
                break;
            }
            else {
                cout << " Ошибка: домашний каталог не найден " << endl;
                break;
            }
        }
        else {
            cout << "ошибка, путь не найден" << endl;
            getline(cin, path);
        }
    }
}

void settings() {
    cout << "дополнительный функционал" << endl;
    //тут можно было бы добавить блок с записью настройки поиска, выбор папки, поиск в подпапках или нет, интервал проверки
}

void read_data() {
    cout << "дополнительный функционал 2" << endl;
    //чтение сохраненного файла, или можно сделать запись 3 последних файлов и читать из них
}

int main() {
    while (true) {
        int choice;

        cout << " 1. запустить скан \n 2. настройки \n 3. чтение записанных файлов \n ввод: " ;
        if (!(cin >> choice)) {
            cout << "ошибка, введите цифру" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
        cin.ignore(10000, '\n');
        switch(choice) {
        case 1:
            search_directory();
            break;
        case 2:
            settings();
            break;
        case 3:
            read_data();
        case 0:
            cout << "завершение программы" << endl;
            break;
        }
    }
    return 0;
}
