/************************************************************************************
* Implementasi program C untuk interface communication menggunakan socket,			*
*																					*
* Program ini membuat socket yang bisa diakses sama program dengan bahasa lain.		*
* Implementasi C ini menggunakan perintah native (bukan menggunakan winsock)		*
* jadinya program ini hanya bisa dirun di LINUX / UNIX.								*
*																					*
* Untuk ngetest ada program JAVA namanya TestClient.java, mereka bisa ngomong2an    *
*																					*
*************************************************************************************/

#include <sys/types.h>		//berisi definisi dari tipe data yang diperlukan sama socket
#include <sys/socket.h>		//berisi beberapa definisi structure yang diperlukan socket
#include <netinet/in.h>		//berisi nilai2 constant dan structure yang diperlukan internet domain addresses
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;

//fungsi buat dapetin ip addressnya client
void getClientAddress(char* outStr, struct sockaddr_in *addr){
	struct sockaddr_in* pV4Addr = addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	inet_ntop( AF_INET, &ipAddr, outStr, INET_ADDRSTRLEN );
}

int main()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int sockfd, newsockfd;	//file descriptor, posisi di array pointer, pointer2 ini nunjuk ke sebuah file disuatu tempat di memori
	int sockopt;			//return value untuk set socket option
	socklen_t clilen;		//ukuran alamat client
	int jumlah_byte;		//return value untuk fungsi read() sama write() nanti
	int portNum = 1500;		//nomor port (ini sebenernya juga bisa di ambil dari argument terminal)
	char buffer[256];       //deklarasi untuk berapa besar ukuran buffer

	//struct sockaddr_in udah bawaan dari netinet/in.h, berikut definisinya
	//  struct sockaddr_in
	//  {
	//    short   sin_family; /* must be AF_INET */
	//    u_short sin_port;
	//    struct  in_addr sin_addr;
	//    char    sin_zero[8]; /* Not used, must be zero */
	//  };
	//--https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
	//variable serv_addr akan berisi alamat dari server
	//variable cli_addr akan berisi alamat dari client yang terkoneksi ke server
	struct sockaddr_in serv_addr, cli_addr;

	/* STEP 1: CREATE SOCKET */

	//buat socket baru, ada dua argument yang dibutuhkan untuk deklarasi socket
	//argument pertama, alamat domain dari socket: AF_INET (specify Internet Family IPv4) dan AF_UNIX (ga tau ini apaan)
	//argument kedua, tipe dari socket: SOCK_STREAM (specify transport layer protocol) dan SOCK_DGRAM (ga tau ini apaan)
	//argument ketiga, protokolnya: kalo 0 system yang bakal pilih protokol yang sesuai, defaultnya TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//cek berhasil apa enggak bikin socketnya
	if (sockfd < 0){
		cout << "ERROR establishing connection." << endl;
		exit(1);
	}
	cout << "Server socket connection created." << endl;

	/* STEP 2: CREATE SOCKET CONFIGURATION */

	//struktur timeval memiliki field second dan microsecond menentukan batas berapa lama menunggu operasi masukan selesai
	struct timeval tv;
	tv.tv_sec = 1;  // 1 second timeout
	tv.tv_usec = 0; // 0 microseconds timeout, kalo ga di declare kadang2 suka geblek

	//setsockopt() untuk menspesifikasikan konfigurasi tertentu, disini kita mau konfigurasi waktu timeout
	//------------ hanya akan bekerja setelah client berhasil terkoneksi
	//------------ akan melakukan konfigurasi opsi socket yang ditentukan oleh:
	//argumen pertama, socket file descriptor
	//argumen kedua, level, untuk melakukan konfigurasi di tingkat socket, konstanta SOL_SOCKET digunakan
	//argumen ketiga, option_name, opsi yang ingin dikonfigurasikan, disini kita menggunakan SO_RCVTIMEO (receive timeout) dan SO_SNDTIMEO (send timeout)
	//argumen keempat, option_value, berupa pointer ke value untuk ngubah opsinya
	//argumen kelima, option_len
	//--http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval)) < 0) perror("FAILED allowing server socket to have receive timeout");
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof(struct timeval)) < 0) perror("FAILED allowing server socket to have send timeout");

	//bzero itu fungsi untuk set nilai value dalam buffer jadi 0
	//jadi maksudnya inisialisasi serv_addr jadi 0
	//http://www.mkssoftware.com/docs/man3/bzero.3.asp
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//inisialisasi socket structure
	serv_addr.sin_family = AF_INET;				//field ini berisi kode untuk address family
	serv_addr.sin_port = htons(portNum);		//buat port number, tapi lebih baik di convert dulu jadi network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY;		//field ini berisi IP address hostnya
												//--https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming

	/* STEP 3: CALL BIND */

	//ngasih rincian yang ditentukan oleh serv_addr (termasuk informasi alamat dan port number) ke socket yang mau dipake
	//--newsockfd "ngebind" jadi sockfd dapat menggunakan alamat tersebut untuk melakukan koneksi ke newsockfd
	//----https://stackoverflow.com/questions/27014955/socket-connect-vs-bind
	//--address current host dan port number di server akan berjalan
	//----http://www.linuxhowtos.org/C_C++/socket.htm
	//argumen pertama, socket file descriptor
	//argumen kedua, pointer ke struct sockaddr tapi yang dilempar merupakan struct sockaddr_in
	//argumen ketiga, size dari address yang akan di bind
	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
		cout << "ERROR binding socket." << endl;
		exit(1);
	}

	/* STEP 4: LISTEN */

	//mengijinkan proses untuk mendengarkan socket buat koneksi
	//argumen pertama, socket file descriptor
	//argumen kedua, ukuran "backlog-queue", banyaknya koneksi yang bisa menunggu selama proses melakkan handling koneksi tertentu
	//-------------- paling besar 5 yang diijinkan oleh kebanyakan sistem
	listen(sockfd, 5);

	/* STEP 5: ACCEPT */

	do{
		//accept() mengakibatkan proses diblok (ditahan) sampai client terkoneksi ke server
		//-------- melanjutkan kembali proses ketika koneksi dari client telah berhasil didirikan
		//-------- mengembalikan file descriptor baru! dan semua komunikasi dalam koneksi harus dilakukan menggunakan file deskirptor baru ini
		//argumen kedua, referensi pointer ke alamat client (ujung koneksi yang satunya)
		//argumen ketiga, ukuran dari strukturnya
		cout << "Trying to connect to clients." << endl;
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	} while(newsockfd < 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "A client is connected." << endl;

	while(true){
		//inisialisasi buffer menggunakan bzero
		bzero(buffer, 256);

//============================================================================================================
		char buffer_send[] = "missanne\n";

		//write byte2 dari string ke client
		//argumen terakhir merakan besar ukuran dari pesan yang mau dikirim
		write(newsockfd, buffer_send, sizeof(buffer_send));
//============================================================================================================


		//baca byte2 dari socket, perlu diingat mulai sekarang kita harus menggunakan file deskriptor yang baru (yang direturn sama fungsi accept())
		//read() akan mengakibatkan proses di blok sampai ada sesuatu yang bisa di baca di socket (contoh: setelah client menjalankan fungsi write())
		//------ akan membaca seluruh karakter di dalam socket atau sejumlah 255
		//------ mengembalikan nilai berapa jumlah byte yang diterima, kalau nilainya -1 artinya gagal menerima data
		jumlah_byte = read(newsockfd, buffer, 255);

		//kalau ada byte dalam buffer maka kita proses
		if (jumlah_byte >= 0) {

			//convert buffernya jadi string, soalnya lebih gampang proses dalam string
			string buffer_str(buffer);
			buffer_str = buffer_str.substr(0, buffer_str.length()-1);

			//kalo mau dapetin alamat clientnya pake fungsi getClientAddress yang gua buat
			char cli_addr_str[INET_ADDRSTRLEN];
			getClientAddress(cli_addr_str, (struct sockaddr_in*) &cli_addr);

			//cetak pesan2nya supaya keliatan sama kita
			cout << "MSG: " << buffer_str << " | FROM: " << cli_addr_str << endl;

			//siapin char array yang berisi pesan yang mau dikirim
			//inget terakhirnya harus ada \n di terakhirnya!
			//njir nyari tau ini sampe 2 jam sendiri, pusing datanya kayak nyangkut di socket, ga selese2 diread sama client
			char buffer_send[] = "I, the server, got your message.\n";

			//write byte2 dari string ke client
			//argumen terakhir merakan besar ukuran dari pesan yang mau dikirim
			jumlah_byte = write(newsockfd, buffer_send, sizeof(buffer_send));

		}

		//kalau ga ada byte maka abaikan aja
		else {
			cout << "Request Timeout." << endl;
		}
	}

	return 0;
}
