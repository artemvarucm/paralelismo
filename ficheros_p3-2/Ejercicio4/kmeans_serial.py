import numpy as np
import pandas as pd
import time
from sklearn.preprocessing import StandardScaler
from numba import njit, get_num_threads, get_thread_id
from numba.openmp import openmp_context as openmp

#from numba import set_num_threads
#set_num_threads(4)

# --- Custom KMeans implementation ---
@njit
def compute_distances(data, centroids, labels):
    n_samples, n_features = data.shape
    k = centroids.shape[0]
    with openmp("parallel for schedule(static, 256)"):
        for i in range(n_samples):
            min_dist = np.inf
            for j in range(k):
                dist = 0.0
                for f in range(n_features):
                    diff = data[i, f] - centroids[j, f]
                    dist += diff * diff
                if dist < min_dist:
                    min_dist = dist
                    labels[i] = j

@njit
def update_centroids(data, labels, k):
    n_samples, n_features = data.shape
    new_centroids = np.zeros((k, n_features))
    counts = np.zeros(k)
    n_threads = get_num_threads()
    local_counts = np.zeros((n_threads, k))
    local_centroids = np.zeros((n_threads, k, n_features))
    with openmp("parallel for shared(local_centroids, local_counts, data, labels) private(c, f, thread_num)"):
        for i in range(n_samples):
            c = labels[i]
            thread_num = get_thread_id()
            with openmp("critical"):
                local_counts[thread_num, c] += 1
                for f in range(n_features):
                    local_centroids[thread_num, c, f] += data[i, f]
        
    with openmp("parallel for firstprivate(n_threads)"):
        for j in range(k):
            for t in range(n_threads):
                new_centroids[j] += local_centroids[t, j]
                counts[j] += local_counts[t, j]

    with openmp("parallel for firstprivate(n_features)"):
        for j in range(k):
            if counts[j] > 0:
                for f in range(n_features):
                    new_centroids[j, f] /= counts[j]

    return new_centroids

def kmeans(data, k=3, max_iters=100):
    n_samples, n_features = data.shape
    centroids = data[np.random.choice(n_samples, k, replace=False)]
    labels = np.zeros(n_samples, dtype=np.int32)

    for _ in range(max_iters):
        compute_distances(data, centroids, labels)
        centroids = update_centroids(data, labels, k)

    return centroids, labels

# --- Load and preprocess data ---
df = pd.read_csv("housing_clean.csv")
data = df.select_dtypes(include=[np.number]).dropna().values
scaler = StandardScaler()
data = scaler.fit_transform(data)
k = 5
# Set a fixed random seed for reproducibility
RANDOM_SEED = 42
np.random.seed(RANDOM_SEED)


# --- Run custom KMeans ---
start_custom = time.time()
custom_centroids, custom_labels = kmeans(data, k)
end_custom = time.time()


# --- Results ---
#print(custom_centroids)
print("Custom KMeans execution time:", round(end_custom - start_custom, 4), "seconds")
print("\nFirst 10 labels (Custom): ", custom_labels[:10])