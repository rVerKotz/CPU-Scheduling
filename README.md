<h2>First-Come, First-Served (FCFS)</h2><br>
<p>
  File FCFS.c menerapkan CPU Scheduling di mana proses yang datang lebih awal akan dieksekusi terlebih dahulu. Kode ini menggunakan kombinasi array dua dimensi, threading, dan semaphore untuk mensimulasikan eksekusi proses secara sinkron. Berikut adalah deskripsi dan detail spesifik kode.
</p>

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
<ul>
  <li>Waiting Time (WT) dihitung berdasarkan waktu tunggu proses sebelum mulai dieksekusi.</li>
  <li>Turnaround Time (TAT) dihitung sebagai selisih waktu dari kedatangan hingga selesai eksekusi.</li>
  <li>Response Time (RT) dihitung sebagai waktu dari kedatangan hingga proses pertama kali mulai berjalan.</li>
</ul>

<h1>Dokumentasi Shortest Remaining Time First (SRTF) Scheduler</h1>
    
    <h2>Pendahuluan</h2>
    <p>Program ini mengimplementasikan algoritma <strong>Shortest Remaining Time First (SRTF)</strong> menggunakan thread di C.</p>
    
    <h2>Struktur Program</h2>
    <ul>
        <li><strong>Fungsi <code>compare</code></strong>: Digunakan untuk mengurutkan tugas berdasarkan waktu burst.</li>
        <pre><code>int compare(const void *a, const void *b) {
    const int *arr1 = (const int *)a;
    const int *arr2 = (const int *)b;
    int sum1 = arr1[3] + arr1[4] + arr1[2];
    int sum2 = arr2[3] + arr2[4] + arr2[2];
    return sum1 - sum2;
}</code></pre>
        <li><strong>Fungsi <code>sort</code></strong>: Digunakan untuk mengurutkan array tugas berdasarkan ID.</li>
        <pre><code>int sort(const void *a, const void *b) {
    const int *arr1 = *(const int **)a;
    const int *arr2 = *(const int **)b;
    return arr1[0] - arr2[0];
}</code></pre>
        <li><strong>Fungsi <code>thread_func</code></strong>: Fungsi utama yang menangani eksekusi setiap thread.</li>
        <pre><code>void *thread_func(void *arg) {
    int *thread_data = (int *)arg;
    pthread_mutex_lock(&mutex[thread_data[0]]);
    for (int i = 0; i < 2; i++) {
        newArr[ctr][i] = thread_data[i];
    }
    ... // Proses perhitungan dan eksekusi thread
    pthread_exit(NULL);
}</code></pre>
        <li><strong>Fungsi <code>signal</code></strong>: Digunakan untuk memberi sinyal kondisi pada thread yang sedang menunggu.</li>
        <pre><code>void signal() {
    for (int j = 0; j < wait_count; j++) {
        pthread_cond_signal(&cond[wait[j]]);
    }
}</code></pre>
        <li><strong>Fungsi <code>main</code></strong>: Menginisialisasi data, membuat thread, dan menghitung waktu eksekusi.</li>
        <pre><code>int main(void) {
    printf("How many times: ");
    scanf("%d", &input);
    ... // Inisialisasi data dan eksekusi thread
    return 0;
}</code></pre>
    </ul>
    
    <h2>Proses Eksekusi</h2>
    <ol>
        <li>Program meminta jumlah tugas dari pengguna.</li>
        <li>Setiap tugas memiliki parameter tertentu yang dimasukkan oleh pengguna.</li>
        <li>Tugas diurutkan berdasarkan prioritas menggunakan SRTF.</li>
        <li>Thread dibuat untuk menangani eksekusi tugas.</li>
        <li>Hasil eksekusi dihitung dan ditampilkan.</li>
    </ol>
    
    <h2>Contoh Input dan Output</h2>
    <h3>Input:</h3>
    <pre>How many times: 3<br/>Task 1: 1 2 3 4<br/>Task 2: 2 1 2 3<br/>Task 3: 3 2 1 2
    </pre>
    
    <h3>Output:</h3>
    <pre>Thread 1: 1 2 3 4<br/>Thread 2: 2 1 2 3<br/>Thread 3: 3 2 1 2<br/>Average = 2.33 1.67 3.00
    </pre>
    
    <h2>Kesimpulan</h2>
    <p>Program ini mengimplementasikan SRTF menggunakan thread dan mutex untuk sinkronisasi proses. Waktu eksekusi dihitung dan dirata-ratakan untuk mendapatkan performa algoritma.</p>
