# Variabili per i percorsi dei file
CIPHER = ../cipher
UTILS_DIR = ../utils
PC_COMMON_DIR = ../pc_common
DM_HASH_DIR = ../dm_hash
PUF_DIR = ../puf
ENROLLMENT_DIR = ./
OBJ_DIR = object

# Elenco dei file oggetto
OBJ := $(addprefix $(OBJ_DIR)/, my_utility.o pc_utils.o common.o encrypt.o encryption_key_schedule.o dm_hash.o decryption_key_schedule.o decrypt.o test_vectors.o utilities.o reedmuller.o dm_hash.o puf.o A_genera_nonce.o)
OBJ1 := $(addprefix $(OBJ_DIR)/, my_utility.o pc_utils.o common.o encrypt.o encryption_key_schedule.o dm_hash.o decryption_key_schedule.o decrypt.o test_vectors.o utilities.o reedmuller.o dm_hash.o puf.o B_genera_nonce.o)
OBJ2 := $(addprefix $(OBJ_DIR)/, my_utility.o pc_utils.o common.o encrypt.o encryption_key_schedule.o dm_hash.o decryption_key_schedule.o decrypt.o test_vectors.o utilities.o reedmuller.o dm_hash.o puf.o genera_chiave.o)

# Target di compilazione
nonce1: $(OBJ)
	gcc $^ -o nonce1

nonce2: $(OBJ1)
	gcc $^ -o nonce2

genera_chiave: $(OBJ2)
	gcc $^ -o genera_chiave

# Target di compilazione
enrollment: $(OBJ)
	gcc $^ -o genera_nonce_dispositivi

# Compilazione dei file oggetto
$(OBJ_DIR)/%.o: $(CIPHER)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(PC_COMMON_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(DM_HASH_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR) -I $(CIPHER) -I $(PC_COMMON_DIR)

$(OBJ_DIR)/%.o: $(PUF_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR) -I $(DM_HASH_DIR)

$(OBJ_DIR)/%.o: $(ENROLLMENT_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR)	-I $(DM_HASH_DIR) -I $(PC_COMMON_DIR) -I $(PUF_DIR)

# Pulizia dei file oggetto
clean:
	rm -f $(OBJ_DIR)/*.o
