# Sistem Monitoring & Kontrol IoT 4 Ruangan Berbasis ESP32, Mosquitto, dan Node-RED (Kelompok 4)

Sistem ini merupakan platform pemantauan suhu dan kelembapan serta kendali saklar lampu jarak jauh secara *real-time* untuk 4 ruangan (`ruang1` s.d. `ruang4`) berbasis protokol MQTT. Setiap node ESP32 mengirimkan telemetri sensor setiap 5 detik, melaporkan status konektivitas (*online/offline*) menggunakan mekanisme *Retained Message* dan *Last Will and Testament* (LWT), serta menerima perintah kontrol 2 lampu LED dari antarmuka **Node-RED Dashboard**. Komunikasi seluruh perangkat dikelola melalui **Broker MQTT Lokal (Eclipse Mosquitto)** yang dilengkapi autentikasi keamanan *username/password* dan dipantau melalui **MQTTX GUI**.

## 1. Anggota Kelompok (`klp4`)

| No | Nama Anggota | NRP | Ruangan yang Dipegang | Sensor & Aktuator Ruangan | 
| ----- | ----- | ----- | ----- | ----- | 
| 1 | **\[Raja Ihsan S\]** | `[2124600108]` | **`ruang1`** | Sensor Digital **AHT10 (I2C)** + 2 LED (`GPIO25`, `GPIO26`) | 
| 2 | **\[Hafiz Ahmad A.\]** | `[2124600106]` | **`ruang2`** | Sensor Analog **Potensiometer (ADC1)** + 2 LED (`GPIO25`, `GPIO26`) | 
| 3 | **\[Faris Raihan F.\]** | `[2124600102]` | **`ruang3`** | Sensor Analog **Potensiometer (ADC1)** + 2 LED (`GPIO25`, `GPIO26`) | 
| 4 | **\[Nadia Salsabila\]** | `[2124600103]` | **`ruang4`** | Sensor Analog **Potensiometer (ADC1)** + 2 LED (`GPIO25`, `GPIO26`) | 

## 2. Link Video Demo & Video Presentasi

* **🎥 Link Video Demonstrasi Sistem (Hardware + Dashboard):** [Klik di Sini untuk Menonton Video Demo Kelompok 4](https://drive.google.com/)

* **🎤 Link Video Presentasi Anggota (Penjelasan Kode & Flow):** [Klik di Sini untuk Menonton Video Presentasi Kelompok 4](https://drive.google.com/)

* **📄 File Tautan Video & Laporan Singkat:** [video/LINK.md](https://www.google.com/search?q=video/LINK.md) | [docs/laporan.pdf](https://www.google.com/search?q=docs/laporan.pdf)

*(Catatan: Akses tautan video sudah diatur ke publik / "Anyone with the link can view").*

## 3. Arsitektur Sistem

Alur komunikasi data dua arah (*publish-subscribe*) antara 4 unit ESP32, Broker Lokal Mosquitto, MQTTX GUI, dan Node-RED Dashboard ditunjukkan pada skema berikut:

```
+---------------------------------------------------------------------------------------------------+
|                                   JARINGAN LOKAL WI-FI / HOTSPOT                                  |
|                                                                                                   |
|  [ESP32 Ruang 1: AHT10 + 2 LED]      ---\                                                         |
|  [ESP32 Ruang 2: Potensio + 2 LED]   ----+--->  [ BROKER LOKAL MOSQUITTO ]  <---> [ MQTTX GUI ]   |
|  [ESP32 Ruang 3: Potensio + 2 LED]   ----+--->  (Host IP Lokal, Port 1883)                        |
|  [ESP32 Ruang 4: Potensio + 2 LED]   ---/       (Auth: kel4 / rahasia123)                         |
|         ^                                                  |                                      |
|         | Subscribe: lamp1, lamp2 (QoS 1)                  | Subscribe Wildcard:                  |
|         | Publish  : temperature, humidity,                | pens/klp4/+/temperature              |
|         |            status (LWT/Retain),                  | pens/klp4/+/humidity                 |
|         |            lamp1/state, lamp2/state              | pens/klp4/+/status                   |
|         |                                                  v                                      |
|         +-------------------------------------  [ NODE-RED FLOW ENGINE ]                          |
|                Publish: lamp1, lamp2                       |                                      |
|                                                            v                                      |
|                                                 [ NODE-RED DASHBOARD UI ]                         |
|                                         (Grafik 4 Garis, 4 Gauge, 8 Switch,                       |
|                                          4 Status LWT, & Overheat Alert >32°C)                    |
+---------------------------------------------------------------------------------------------------+

```

### Struktur Direktori Repositori (`Kel4-TugasNodeRED-ESP32/`)

| Path File / Folder | Deskripsi | 
| ----- | ----- | 
| `README.md` | Halaman utama dokumentasi proyek (12 poin wajib Bagian 10.2) | 
| `esp32/esp32_ruang/` | Proyek PlatformIO / Arduino (`.ino` / `src/main.cpp`) untuk ke-4 ruangan | 
| `node-red/flows.json` | File *export* alur Node-RED berbasis *Wildcard* dan *Function Router* | 
| `docs/laporan.pdf` | Laporan singkat praktikum dan evaluasi sistem | 
| `docs/images/` | Folder dokumentasi foto rangkaian (`rangkaian.jpg`), *flow* (`flow.png`), dan *dashboard* (`dashboard.png`) | 
| `video/LINK.md` | Berisi tautan video demonstrasi alat dan video presentasi kelompok | 

## 4. Perangkat Keras

### Daftar Komponen Perangkat Keras

* **Mikrokontroler:** 4× Board ESP32 DevKit V1 (Wi-Fi + Dual-Core + ADC 12-bit + I2C).

* **Sensor Ruang 1:** 1× Modul Sensor Suhu & Kelembapan Digital **AHT10** (antarmuka I2C).

* **Sensor Ruang 2, 3, dan 4:** 3× **Potensiometer 10 kΩ** sebagai simulator sensor suhu dan kelembapan presisi.

* **Aktuator Lampu:** 8× LED (2 lampu per ruangan: Lampu 1 & Lampu 2) + 8× Resistor 220 Ω.

* **Pendukung:** 4× Breadboard, kabel *jumper* Male-to-Male / Male-to-Female, dan 4× kabel data USB.

### Tabel Konfigurasi Pin ESP32

| Ruangan | Komponen | Pin Komponen | Pin ESP32 | Keterangan Teknis | 
| ----- | ----- | ----- | ----- | ----- | 
| **Ruang 1** | Sensor **AHT10 (I2C)** | `VIN` / `GND`  `SDA` (Data)  `SCL` (Clock) | `3.3V` / `GND`  `GPIO 21`  `GPIO 22` | Komunikasi I2C standar pada alamat `0x38` menggunakan pustaka `Adafruit_AHTX0`. | 
| **Ruang 2, 3, 4** | **Potensiometer 10 kΩ** | Kaki Kiri & Kanan  Kaki Tengah (*Wiper*) | `3.3V` & `GND`  `GPIO 34` (ADC1) | Wajib memakai kanal **ADC1** (`GPIO 32–39`) karena kanal ADC2 tidak dapat dipakai saat Wi-Fi aktif. | 
| **Semua Ruang (1–4)** | **LED Lampu 1** | Anoda (+) / Katoda (-) | `GPIO 25` / `GND` | Dihubungkan seri dengan resistor 220 Ω (`"1"` = `HIGH`, `"0"` = `LOW`). | 
| **Semua Ruang (1–4)** | **LED Lampu 2** | Anoda (+) / Katoda (-) | `GPIO 26` / `GND` | Dihubungkan seri dengan resistor 220 Ω (`"1"` = `HIGH`, `"0"` = `LOW`). | 

### Foto Rangkaian Perangkat Keras Satu Ruangan

## 5. Tabel Topic Final & Konfigurasi Broker

### Detail Broker MQTT yang Dipakai

* **Perangkat Lunak Broker:** Eclipse Mosquitto (Self-Hosted Local Broker)

* **Host / IP Broker:** IPv4 Wi-Fi Laptop Broker (untuk koneksi ESP32) / `127.0.0.1` (untuk koneksi Node-RED & MQTTX lokal)

* **Port TCP:** `1883`

* **Keamanan Autentikasi (Nilai Tambah):** Aktif (`allow_anonymous false` + `password_file`, Username: `kel4`)

* **Prefix Kelompok:** `pens/klp4/`

### Tabel Daftar Topik MQTT (`ruang1` s.d. `ruang4`)

| Topik MQTT Final | Publisher | Subscriber | Contoh Payload | QoS | Retain | Keterangan Fungsi | 
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | 
| `pens/klp4/ruang1/temperature`  *(s.d. `ruang4/temperature`)* | ESP32 | Node-RED (`pens/klp4/+/temperature`) | `28.5` | 0 | Tidak | Mengirimkan nilai suhu (°C, 1 angka desimal) setiap 5 detik. | 
| `pens/klp4/ruang1/humidity`  *(s.d. `ruang4/humidity`)* | ESP32 | Node-RED (`pens/klp4/+/humidity`) | `65.2` | 0 | Tidak | Mengirimkan nilai kelembapan (%RH, 1 angka desimal) setiap 5 detik. | 
| `pens/klp4/ruang1/lamp1`  *(s.d. `ruang4/lamp1`)* | Node-RED | ESP32 (`ruangN`) | `1` / `0` | 1 | Tidak | Perintah menyalakan (`1`) atau mematikan (`0`) LED Lampu 1 (`GPIO25`). | 
| `pens/klp4/ruang1/lamp2`  *(s.d. `ruang4/lamp2`)* | Node-RED | ESP32 (`ruangN`) | `1` / `0` | 1 | Tidak | Perintah menyalakan (`1`) atau mematikan (`0`) LED Lampu 2 (`GPIO26`). | 
| `pens/klp4/ruang1/status`  *(s.d. `ruang4/status`)* | ESP32 / Broker | Node-RED (`pens/klp4/+/status`) | `online` / `offline` | 0 | **Ya** | Status perangkat; otomatis berubah menjadi `offline` oleh LWT broker jika ESP32 putus. | 
| `pens/klp4/ruang1/lamp1/state`  *(s.d. `ruang4/lamp2/state`)* | ESP32 | Node-RED (`pens/klp4/+/+/state`) | `1` / `0` | 0 | **Ya** | **(Fitur Nilai Tambah)** Umpan balik kondisi aktual LED ke saklar *dashboard*. | 

## 6. Cara Menjalankan Sistem

### A. Persiapan Library & Langkah Upload ke ESP32 (`esp32/esp32_ruang/`)

1. **Daftar Library yang Dibutuhkan:**

   * `EspMQTTClient` (oleh Patrick Lapointe) — mengelola koneksi Wi-Fi, MQTT, Auto-Reconnect, dan LWT.

   * `PubSubClient` (oleh Nick O'Leary) — dependensi komunikasi MQTT.

   * `Adafruit AHTX0`, `Adafruit Unified Sensor`, dan `Adafruit BusIO` — khusus pembacaan sensor I2C AHT10 pada Ruang 1.

2. **Bagian Kode yang Diganti Sebelum Upload ke Masing-Masing ESP32:**
   Buka kode sumber di dalam folder `esp32/esp32_ruang/` dan sesuaikan konstanta berikut untuk setiap ruangan:

   * **`KELOMPOK`:** Pastikan bernilai `"klp4"`.

   * **`ROOM`:** Ganti sesuai board yang di-upload, yaitu `"ruang1"`, `"ruang2"`, `"ruang3"`, atau `"ruang4"`.

   * **Kredensial Wi-Fi & IP Broker pada objek `EspMQTTClient client(...)`:**

     * Ganti `"NAMA_WIFI_ANDA"` dan `"PASSWORD_WIFI_ANDA"` sesuai SSID dan kata sandi Wi-Fi/Hotspot yang digunakan.

     * Ganti alamat IP broker (mis. `"192.168.1.XX"`) dengan alamat IPv4 Wi-Fi laptop tempat Mosquitto dijalankan.

     * Pastikan *username* (`"kel4"`) dan *password* MQTT sesuai dengan yang didaftarkan pada file `passwd.txt` Mosquitto.

3. Hubungkan ESP32 ke komputer melalui kabel USB, lalu tekan tombol **Upload (`→`)** pada PlatformIO / Arduino IDE dan pantau koneksi melalui **Serial Monitor** (`115200 baud`).

### B. Cara Menjalankan Broker Lokal Mosquitto

Jalankan perintah berikut pada *Command Prompt* (Administrator):

```
net stop mosquitto
"E:\02_SoftwareKerja\MosquitoMQTTX\Mosquitto\mosquitto.exe" -c "E:\02_SoftwareKerja\MosquitoMQTTX\Mosquitto\mosquitto.conf" -v

```

### C. Cara Import `node-red/flows.json` ke Node-RED & Mengakses Dashboard

1. Jalankan Node-RED melalui terminal dengan mengetikkan `node-red`, lalu buka editor pada alamat `http://localhost:1880`.

2. **Pasang Paket Palette Dashboard:** Klik menu garis tiga (`☰`) di pojok kanan atas → **Manage palette** → tab **Install** → cari dan pasang paket **`@flowfuse/node-red-dashboard`** (atau `node-red-dashboard`).

3. **Import File `node-red/flows.json`:** Klik menu (`☰`) → **Import** → **select a file to import** → pilih file **`node-red/flows.json`** dari repositori ini → klik **Import**.

4. **Atur Kredensial Broker:** Klik dua kali salah satu node `mqtt in`, klik ikon pensil pada **Server** (`Local Mosquitto Broker`), buka tab **Security**, isi **Username** `kel4` beserta **Password**, klik **Update**, lalu tekan tombol merah **Deploy**.

5. **Buka Halaman Dashboard:**

   * Untuk pengguna `@flowfuse/node-red-dashboard` (Dashboard 2.0): akses **`http://localhost:1880/dashboard`**

   * Untuk pengguna `node-red-dashboard` (Dashboard 1.0): akses **`http://localhost:1880/ui`**

## 7. Screenshot Flow Node-RED & Dashboard

### Tampilan Editor Flow Node-RED (Wildcard + Function Router)

### Tampilan Antarmuka Node-RED Command Center Dashboard

## 8. Hasil Pengujian Sistem (T1 – T12)

| Kode Uji | Skenario Pengujian | Status (✓ / ✗) | Keterangan Singkat Hasil Pengamatan | 
| ----- | ----- | ----- | ----- | 
| **T1** | **Data sensor 4 ruangan tampil di grafik & gauge** | **✓** | Grafik garis menampilkan 4 kurva suhu (`RUANG1`–`RUANG4`) yang diperbarui tiap 5 detik; ke-4 *gauge* kelembapan bergerak responsif saat AHT10 disentuh atau potensiometer diputar. | 
| **T2** | **Kontrol Lampu 1 pada salah satu ruangan (ON/OFF)** | **✓** | Saat saklar `💡 Lampu 1` Ruang 2 ditekan ke ON (`"1"`) lalu OFF (`"0"`), LED pada `GPIO25` ESP32 Ruang 2 menyala dan padam seketika tanpa jeda. | 
| **T3** | **Independensi kontrol lampu antar-ruangan** | **✓** | Pengubahan saklar `Lampu 1` dan `Lampu 2` pada suatu ruangan hanya memengaruhi LED di ruangan tersebut; ruangan lain tidak ikut berubah. | 
| **T4** | **Status Online langsung tampil saat halaman baru dibuka (*Retained*)** | **✓** | Ketika tab browser baru dibuka ke halaman dashboard saat ESP32 sudah menyala, keempat indikator status langsung menampilkan `🟢 ONLINE (Tersambung)` berkat *flag* `retain = true`. | 
| **T5** | **Deteksi perangkat mati mendadak dengan LWT (`offline`)** | **✓** | Saat kabel USB ESP32 Ruang 3 dicabut paksa, dalam ±15 detik broker Mosquitto mengirimkan pesan LWT sehingga status Ruang 3 di dashboard otomatis berubah menjadi `🔴 OFFLINE (Terputus)`. | 
| **T6** | **Pemulihan koneksi otomatis (*Auto-Reconnect*)** | **✓** | Saat ESP32 Ruang 3 dinyalakan kembali, perangkat otomatis menyambung ulang ke Wi-Fi & broker lokal tanpa perlu *refresh* halaman dashboard, dan status kembali `🟢 ONLINE`. | 
| **T7** | **Isolasi topik dari kelompok lain** | **✓** | Pengiriman pesan uji ke topik `pens/klp99/ruang1/temperature` dari MQTTX diabaikan sepenuhnya dan tidak mencemari grafik maupun *gauge* Kelompok 4 (`klp4`). | 
| **T8** | **Pemakaian *Wildcard* (`+`) pada sisi penerima Node-RED** | **✓** | Node-RED cukup menggunakan 4 node `mqtt in` ber-*wildcard* (`pens/klp4/+/temperature`, `+/humidity`, `+/status`, `+/+/state`) yang dirutekan dengan node `function`. | 
| **T9** | **Interval pengiriman non-blocking (`millis()`) 5 detik** | **✓** | Pembacaan sensor menggunakan `millis() - lastSend >= 5000` tanpa `delay()` panjang, sehingga penerimaan perintah lampu dari dashboard tetap instan setiap saat. | 
| **T10** | **Konfirmasi status lampu dua arah (*State Feedback* `lamp/state`)** | **✓** | Saat perintah lampu dikirim lewat MQTTX, ESP32 menyalakan LED lalu mem-publish balik ke `pens/klp4/ruangN/lampM/state` (`retain=true`) sehingga posisi saklar di dashboard ikut sinkron. | 
| **T11** | **Peringatan suhu tinggi otomatis (*Overheat Notification* > 32 °C)** | **✓** | Ketika suhu ruangan melebihi `32,0 °C` (mis. potensiometer diputar ke `35,0 °C`), muncul *popup toast notification* peringatan merah di pojok kanan atas dashboard. | 
| **T12** | **Keamanan autentikasi Broker Lokal Mosquitto** | **✓** | Klien MQTTX atau ESP32 tanpa *username* (`kel4`) dan *password* yang benar ditolak oleh broker (`Connection Refused: not authorized`). | 

## 9. Ruangan yang Disimulasikan & Penyesuaian Sensor

* **Penggunaan Sensor Berbeda (AHT10 & Potensiometer):** Karena keterbatasan ketersediaan sensor DHT11/DHT22, **Ruang 1** menggunakan sensor suhu dan kelembapan digital **AHT10 (I2C)**, sedangkan **Ruang 2, 3, dan 4** menggunakan **Potensiometer (ADC1 `GPIO34`)** sebagai simulator nilai suhu (`18,0 °C – 40,0 °C`) dan kelembapan relatif terkompensasi (`82,0 %RH – 38,0 %RH`). Seluruh ruangan tetap mengirimkan format payload string angka desimal yang identik ke topik MQTT masing-masing.

* **Skenario Simulasi Jika Perangkat Fisik Kurang dari 4 Unit Pada Saat Pengujian Mandiri:** Apabila pada saat pengujian awal terdapat ruangan yang belum terhubung ke board ESP32 fisik, pengujian aliran data dilakukan dengan menyimulasikan publikasi topik ruangan tersebut melalui **MQTTX GUI** atau perintah `mosquitto_pub` ke broker lokal sehingga keempat garis grafik dan keempat *gauge* ruangan tetap terverifikasi penuh.

## 10. Kendala dan Solusi

| No | Kendala yang Dihadapi Selama Pengerjaan | Penyebab Teknis | Solusi yang Diterapkan | 
| ----- | ----- | ----- | ----- | 
| 1 | **ESP32 gagal terhubung ke Broker Mosquitto di laptop (`Connection failed`)** | Konfigurasi bawaan Mosquitto 2.0+ hanya membuka *listener* pada `localhost` (`127.0.0.1`) dan diblokir oleh Windows Firewall. | Menambahkan `listener 1883 0.0.0.0`, `allow_anonymous false`, dan `password_file` pada `mosquitto.conf`, serta membuka port TCP `1883` di Windows Firewall. | 
| 2 | **Pembacaan analog potensiometer tidak stabil / gagal saat Wi-Fi ESP32 aktif** | Kanal **ADC2** pada ESP32 digunakan bersama oleh perangkat keras driver Wi-Fi sehingga tidak bisa dipakai membaca sensor analog ketika Wi-Fi menyala. | Memindahkan pin input potensiometer ke kanal **ADC1 (`GPIO 34`)** dan menambahkan *multisampling* rata-rata 16 sampel agar nilai suhu stabil. | 
| 3 | **Saklar lampu di Node-RED tidak menyalakan LED dan terasa berat saat diklik** | Terdapat spasi di awal topik `mqtt out` (`" pens/klp4/..."`), payload saklar masih mengirim `"true"`/`"false"`, dan fitur `decouple` menahan perubahan saklar. | Menghapus spasi pada topik, mengubah payload saklar menjadi string `"1"` dan `"0"`, serta mengatur `decouple: false` dan `clickableArea: "line"`. | 
| 4 | **Tampilan *Gauge* di Dashboard 2.0 terlalu kecil dan busurnya berwarna merah semua** | Ukuran widget masih `width: 4, height: 2` dan batas warna segmen tertulis `0, 4, 7` (bukan `0, 35, 75`). | Mengubah ukuran *Gauge* menjadi `width: 6, height: 5` dengan tipe `gauge-34` dan memperbaiki ambang batas warna segmen ke `0%`, `35%`, dan `75%`. | 
| 5 | **Terlalu banyak node `mqtt in` dan `mqtt out` membuat area kerja Node-RED penuh** | Setiap ruangan dan setiap lampu awalnya dibuatkan node MQTT sendiri-sendiri (total >25 node MQTT). | Meringkas *flow* menggunakan **4 node `mqtt in` ber-*wildcard* (`+`)** yang dipadukan dengan node **`function` Router** dan **1 node `mqtt out` dinamis** berbasis `msg.topic`. | 

## 11. Pembagian Tugas Anggota Kelompok

| Nama Anggota | Ruangan | Rincian Tugas & Kontribusi | 
| ----- | ----- | ----- | 
| **\[Raja Ihsan S.\]** | `ruang1` | Merakit rangkaian ESP32 Ruang 1 dengan sensor I2C **AHT10** dan 2 LED, menulis program pembacaan sensor non-blocking `millis()`,menulis dan mengaplikasikan node-red dan dashboard, membantu pengambilan dan mengedit video demo, serta menyusun `README.md`. | 
| **\[Hafiz Ahmad A.\]** | `ruang2` | Merakit rangkaian ESP32 Ruang 2 dengan **Potensiometer (ADC1 `GPIO34`)** dan 2 LED, Mengambil Video (Cameramen) , serta membuat dokumentasi PPT dan Laporan. | 
| **\[Faris Raihan F.\]** | `ruang3` | Merakit rangkaian ESP32 Ruang 3, membantu menjadi operator dalam take video demo, menguji fitur pembukaan dashboard lewat android, serta membantu pembuatan PPT dan Laporan . | 
| **\[Nadia Salsabila\]** | `ruang4` | Merakit rangkaian ESP32 Ruang 4, membantu menjadi aktor dalam pengambilan video demo ruang 1, membuat script dalam pengambilan video presesntasi, serta mengedit video presentasi. | 
