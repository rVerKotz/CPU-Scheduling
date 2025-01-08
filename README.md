  1. First-Come, First-Served (FCFS) <br>
File FCFS.c menerapkan CPU Scheduling di mana proses yang datang lebih awal akan dieksekusi terlebih dahulu. Kode ini menggunakan kombinasi array dua dimensi, threading, dan semaphore untuk mensimulasikan eksekusi proses secara sinkron. Berikut adalah deskripsi dan detail spesifik kode.

  1.1 Input dan Persiapan Data<br>
Bagian awal program meminta pengguna untuk memasukkan jumlah proses dan informasi terkait setiap proses, yaitu Task ID, Arrival Time, Burst Time, dan Priority. Informasi ini disimpan dalam array dua dimensi arr, di mana setiap baris merepresentasikan satu proses:

```
int arr[input][4]; // Array 2D untuk menyimpan proses
for (int j = 0; j < input; j++) {
    printf("Task %d: ", j + 1);
    for (int i = 0; i < 4; i++) {
        scanf("%d", &arr[j][i]);
    }
}
```
Setelah input diterima, array diurutkan berdasarkan waktu kedatangan menggunakan fungsi qsort dengan pembanding compare, untuk memastikan proses dieksekusi dalam urutan yang benar.
```
int compare(const void *a, const void *b) {
    const int *arr1 = (const int *)a;
    const int *arr2 = (const int *)b;
    return arr1[1] - arr2[1]; // Mengurutkan berdasarkan Arrival Time
}
```

  1.2 Inisialisasi Semaphore dan Thread <br>
Setiap proses direpresentasikan oleh sebuah thread. Sinkronisasi antar-thread dilakukan menggunakan semaphore. Array semaphore print_semaphores diinisialisasi dengan nilai awal 0 untuk semua proses, kecuali proses pertama yang dilepas dengan sem_post.

```
print_semaphores = malloc((max_thread_id + 1) * sizeof(sem_t));
for (int i = 0; i < input; i++) {
    sem_init(&print_semaphores[arr[i][0]], 0, 0);
}
sem_post(&print_semaphores[arr[0][0]]);
```
  1.3 Fungsi Thread untuk Eksekusi Proses <br>
Setiap thread memproses satu proses sesuai waktu kedatangan dan burst time. Sinkronisasi dijamin dengan semaphore:

```
void *thread(void *arg) {
    int *thread_data = (int*)arg; // Mengambil data proses
    sem_wait(&print_semaphores[thread_data[0]]); // Menunggu izin eksekusi

    printf("Thread %d: ", ctr+1);
    for (int i = 0; i < 4; i++) {
        if (i == 0) {
            printf("%d ", thread_data[i]); // Menampilkan Task ID
        } 
        else if (i == 3) {
            result += thread_data[i];
            TAT[ctr] = result - TAT[ctr]; // Menghitung Turnaround Time
            printf("%d ", result);
        }
        else {
            if (i == 1){
                TAT[ctr] = thread_data[i];
                RT[ctr] -= thread_data[i]; // Menghitung Response Time
            }
            printf("%d ", result);
            result += thread_data[i];
        }
    }
    printf("\n");

    // Memberi izin ke thread proses berikutnya
    if (ctr < input){
        sem_post(&print_semaphores[thread_data[4]]);
        ctr++;
        WT[ctr] = result;
        RT[ctr] = result;
    }

    pthread_exit(NULL); // Thread selesai
}
```

  1.4 Perhitungan Waktu <br>
• Waiting Time (WT) dihitung berdasarkan waktu tunggu proses sebelum mulai dieksekusi.
• Turnaround Time (TAT) dihitung sebagai selisih waktu dari kedatangan hingga selesai eksekusi.
• Response Time (RT) dihitung sebagai waktu dari kedatangan hingga proses pertama kali mulai berjalan.
