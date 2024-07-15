#include <iostream>

#include <windows.h>

#include <mysql.h>

#include <fstream>

#include <sstream>

#include <ctime>

#include <cstring>

#include <array>

#include <string>

#include <iphlpapi.h>

#include <ws2tcpip.h>

#include <cstdio>

#include <vector>
 //-----------------------
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#define MIB_IF_TYPE_802_11_WIRELESS 71

using namespace std;

void hello(string text) {
  std::cout << text << endl;
}

////////////////////////////////////////////////// 2- Operation calcul
void operation(int a = 0, int b = 0, string op = "+") {
  try {
    float result = 0;
    if (op == "+") {
      // Somme
      result = a + b;
    } else if (op == "-") {
      // Soustraction
      result = a - b;
    } else if (op == "*") {
      // Multiplication
      result = a * b;
    } else if (op == "/") {
      // Division
      if (b != 0) {
        result = a / b;
      } else {
        std::cout << "Erreur div sur 0 " << endl;
        return;
      }
    } else {
      result = 0;
    }
    std::cout << "Le resultat de A " + op + " B est : ( " << result << " )" << endl;
  } catch (const std::exception & ) {
    std::cout << "Erreur " << endl;
    return;
  }
}
////////////////////////////////////////////////// 3- Switch
void switch_func(int a = 0, int b = 0) {
  int r;
  if (a > b) {
    r = 2;
  } else if (a == b) {
    r = 1;
  } else {
    r = 0;
  }
  switch (r) {
  case 2:
    cout << "'a' est supp a 'b' " << endl;
    break;
  case 1:
    cout << "'a' egale 'b' " << endl;
    break;
  case 0:
    cout << "'a' est inf a 'b' " << endl;
    break;
  default:
    break;
  }
}
////////////////////////////////////////////////// 4- Boucles
void boucles_func(int a = 5) {
  int i = 0;
  while (i < a) {
    cout << "Salut " << i << " fois \n" <<
      endl;
    i++;
  }
}
////////////////////////////////////////////////// 5- Pointeurs

string getDateTime() {
  std::time_t now = std::time(NULL);
  std::tm * localTime = std::localtime( & now);
  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
  return string(buffer);
}

std::string getMacAddress() {
  IP_ADAPTER_INFO AdapterInfo[16];
  DWORD dwBufLen = sizeof(AdapterInfo);

  DWORD dwStatus = GetAdaptersInfo(
    AdapterInfo, &
    dwBufLen);

  if (dwStatus != ERROR_SUCCESS) {
    return "Error: Unable to get adapter info";
  }

  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
  std::ostringstream oss;
  oss << std::hex;

  for (UINT i = 0; i < pAdapterInfo -> AddressLength; ++i) {
    if (i != 0)
      oss << ":";
    oss << (pAdapterInfo -> Address[i] < 16 ? "0" : "") << (int) pAdapterInfo -> Address[i];
  }

  return oss.str();
}

std::string getIPv4Address() {
  std::string ipAddress = "0.0.0.0";
  DWORD size = 0;
  PIP_ADAPTER_INFO info = nullptr;

  if (GetAdaptersInfo(nullptr, & size) == ERROR_BUFFER_OVERFLOW) {
    info = (IP_ADAPTER_INFO * ) malloc(size);
    if (info == nullptr) {
      std::cerr << "Memory allocation error\n";
      return ipAddress;
    }

    if (GetAdaptersInfo(info, & size) == NO_ERROR) {
      PIP_ADAPTER_INFO adapter = info;
      while (adapter) {
        if (adapter -> Type == MIB_IF_TYPE_802_11_WIRELESS && adapter -> IpAddressList.IpAddress.String[0] != '\0' &&
          strcmp(adapter -> IpAddressList.IpAddress.String, "0.0.0.0") != 0) {
          ipAddress = adapter -> IpAddressList.IpAddress.String;
          break;
        }
        adapter = adapter -> Next;
      }
    } else {
      std::cerr << "GetAdaptersInfo failed\n";
    }
    free(info);
  } else {
    std::cerr << "GetAdaptersInfo failed to get size\n";
  }
  return ipAddress;
}

int get_info_ram_size() {
  int ram_size = 0;

  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);

  if (GlobalMemoryStatusEx( & statex)) {
    DWORDLONG totalMemory = statex.ullTotalPhys;
    ram_size = totalMemory / (1024 * 1024);
    return ram_size;
  } else {
    std::cerr << "Erreur get_info_ram_size " << GetLastError() << std::endl;
  }

  return ram_size;
}
int get_info_ram_used() {
  int ram_used = 0;
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);

  if (GlobalMemoryStatusEx( & statex)) {
    DWORDLONG totalMemory = statex.ullTotalPhys;
    DWORDLONG memoryInUse = totalMemory - statex.ullAvailPhys;
    ram_used = memoryInUse / (1024 * 1024);
    return ram_used;
  } else {
    std::cerr << "Erreur get_info_ram_used " << GetLastError() << std::endl;
  }
  return ram_used;
}

int get_info_disk_size() {
  int disk_size = 0;

  ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;

  if (GetDiskFreeSpaceEx(NULL, & freeBytesAvailable, & totalNumberOfBytes, & totalNumberOfFreeBytes)) {
    DWORDLONG totalDiskSize = totalNumberOfBytes.QuadPart;
    disk_size = int(totalDiskSize / (1024 * 1024));
    return disk_size;
  } else {
    std::cerr << "Erreur get_info_disk_size " << GetLastError() << std::endl;
  }

  return disk_size;
}

int get_info_storage_used() {
  int disk_used = 0;
  ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;

  if (GetDiskFreeSpaceEx(NULL, & freeBytesAvailable, & totalNumberOfBytes, & totalNumberOfFreeBytes)) {
    DWORDLONG totalDiskSize = totalNumberOfBytes.QuadPart;
    DWORDLONG freeDiskSpace = freeBytesAvailable.QuadPart;

    // Calculate used storage in bytes
    DWORDLONG usedDiskSpace = totalDiskSize - freeDiskSpace;

    // Convert used storage to megabytes
    int disk_used = static_cast < int > (usedDiskSpace / (1024 * 1024));

    return disk_used;
  } else {
    std::cerr << "Error get_info_storage_used: " << GetLastError() << std::endl;
  }
  return disk_used;
}

void device_info() {
  cout << "La date actuelle de la machine est : " << getDateTime();
  cout << "\n";
  cout << "\n";
  cout << "IP de la machine est : " << getIPv4Address();
  cout << "\n";
  cout << "\n";
  cout << "Adresse MAC de la machine est : " << getMacAddress() << " MB";
  cout << "\n";
  cout << "\n";
  cout << "Taille de la RAM Totale : " << get_info_ram_size() << " MB";
  cout << "\n";
  cout << "\n";
  cout << "Taille de la RAM utilisee : " << get_info_ram_used() << " MB";
  cout << "\n";
  cout << "\n";
  cout << "Taille total de disque : " << get_info_disk_size() << " MB";
  cout << "\n";
  cout << "\n";
  cout << "Taille de disque utilise : " << get_info_storage_used() << " MB";
  cout << "\n";
  cout << "\n";

}

void insert_in_database() {

  std::string dateTime = getDateTime();
  std::string ipAddress = getIPv4Address();
  std::string macAddress = getMacAddress();
  int ram_size = get_info_ram_size();
  int ram_used = get_info_ram_used();
  int disk_size = get_info_disk_size();
  int storage_used = get_info_storage_used();

  string entree;
  MYSQL * conn;
  conn = mysql_init(0);

  conn = mysql_real_connect(conn, "127.0.0.1", "root", "", "oreilly_cpp", 0, NULL, 0);
  if (conn) {
    int qstate = 0;

    stringstream ss;
    ss << "INSERT INTO device_info(mac_address, ip_address,ram_size,ram_used,storage_size,storage_used, date_time,date_commande) VALUES('" <<
      macAddress << "','" << ipAddress << "','" << ram_size << "','" << ram_used << "','" << disk_size << "','" << storage_used << "','" << dateTime << "',NOW())";
    cout << "\n";
    cout << "\n";
    string query = ss.str();
    cout << query << endl;
    const char * q = query.c_str();
    qstate = mysql_query(conn, q);
    if (qstate == 0) {
      cout << "Ligne inseree \n\n" <<
        endl;
      //system("pause");
    } else {
      cout << "Erreur d'insertion \n\n" <<
        mysql_error(conn) << endl;
      //system("pause");
    }
  } else {
    cout << "Erreur de Connection avec la base de donnees" << endl;
    //system("pause");
  }
  //system("cls");
}

int main() {
  int choix;

  while (true) {
    std::cout << "Selectionnez la commande a excecuter \n";

    std::cout << "1- Operation de calcul/SWITCH/BOUCLES FOR\n";
    std::cout << "2- Informations systeme \n";
    std::cout << "3- Inserer sur Base de donnees\n";
    std::cout << "0- Quitter\n ";
    std::cin >> choix;

    switch (choix) {
    case 1:

      int a;
      cout << "Saisissez A 'Chiffre' : ";
      cin >> a;
      int b;
      cout << "Saisissez b 'Chiffre': ";
      cin >> b;

      operation(a, b, "+");

      operation(a, b, "-");

      operation(a, b, "*");

      operation(a, b, "/");

      switch_func(a, b);

      boucles_func();
      break;
    case 2:
      device_info();
      break;
    case 3:
      insert_in_database();
      break;
    case 0:
      return 0;
    default:
      return 0;
    }
  }
}
////////////////////////////////////////////////// Fin Main
