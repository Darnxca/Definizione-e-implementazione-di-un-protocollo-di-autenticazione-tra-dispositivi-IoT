# Variabili per i percorsi dei file
CIPHER_DIR = ../cipher
UTILS_DIR = ../utils
PC_COMMON_DIR = ../pc_common
DM_HASH_DIR = ../dm_hash
PUF_DIR = ../puf
HKDF_DIR = ../hkdf
ENROLLMENT_DIR = ./
OBJ_DIR = object

# Elenco dei file oggetto
OBJ := $(addprefix $(OBJ_DIR)/, usha.o sha384-512.o sha224-256.o sha1.o hmac.o hkdf.o my_utility.o pc_utils.o common.o encrypt.o encryption_key_schedule.o dm_hash.o decryption_key_schedule.o decrypt.o test_vectors.o utilities.o reedmuller.o dm_hash.o puf.o main.o)

# Target di compilazione
enrollment: $(OBJ)
	gcc $^ -o ottenimento_nonceAB

# Compilazione dei file oggetto
$(OBJ_DIR)/%.o: $(CIPHER_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(PC_COMMON_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(DM_HASH_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR) -I $(CIPHER_DIR) -I $(PC_COMMON_DIR)

$(OBJ_DIR)/%.o: $(PUF_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR) -I $(DM_HASH_DIR)

$(OBJ_DIR)/%.o: $(HKDF_DIR)/%.c
	gcc -c $< -o $@ -I $(HKDF_DIR)

$(OBJ_DIR)/%.o: $(ENROLLMENT_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR)	-I $(DM_HASH_DIR) -I $(PC_COMMON_DIR) -I $(PUF_DIR) -I $(HKDF_DIR) -I $(CIPHER_DIR)

# Pulizia dei file oggetto
clean:
	rm -f $(OBJ_DIR)/*.o
