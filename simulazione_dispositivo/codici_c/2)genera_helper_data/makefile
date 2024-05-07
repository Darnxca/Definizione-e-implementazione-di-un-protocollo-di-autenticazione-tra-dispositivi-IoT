# Variabili per i percorsi dei file
CIPHER = ../cipher
UTILS_DIR = ../utils
PC_COMMON_DIR = ../pc_common
DM_HASH_DIR = ../dm_hash
ENROLLMENT_DIR = ./
OBJ_DIR = object

# Elenco dei file oggetto
OBJ := $(addprefix $(OBJ_DIR)/, my_utility.o pc_utils.o common.o encrypt.o encryption_key_schedule.o dm_hash.o decryption_key_schedule.o decrypt.o test_vectors.o utilities.o reedmuller.o main.o)

# Target di compilazione
enrollment: $(OBJ)
	gcc $^ -o genera_helper_data

# Compilazione dei file oggetto
$(OBJ_DIR)/%.o: $(CIPHER)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(PC_COMMON_DIR)/%.c
	gcc -c $< -o $@

$(OBJ_DIR)/%.o: $(DM_HASH_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR) -I $(CIPHER) -I $(PC_COMMON_DIR)

$(OBJ_DIR)/%.o: $(ENROLLMENT_DIR)/%.c
	gcc -c $< -o $@ -I $(UTILS_DIR)	-I $(DM_HASH_DIR) -I $(PC_COMMON_DIR)

# Pulizia dei file oggetto
clean:
	rm -f $(OBJ_DIR)/*.o
