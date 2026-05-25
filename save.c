#include "tamagotchi.h"
#include "save.h"
#include <storage/storage.h>

#define SAVE_MAGIC 0x43525452u // "CRTR"
#define SAVE_VERSION 4u
#define SAVE_PATH APP_DATA_PATH("critter.sav")

typedef struct {
    uint32_t magic;
    uint32_t version;
    Pet pet;
    Settings settings;
} SaveBlob;

void save_load(TamagotchiApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    SaveBlob blob;

    bool ok = false;
    if(storage_file_open(file, SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        size_t read = storage_file_read(file, &blob, sizeof(blob));
        ok = (read == sizeof(blob)) && blob.magic == SAVE_MAGIC && blob.version == SAVE_VERSION;
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    if(ok) {
        app->pet = blob.pet;
        app->settings = blob.settings;
    }
}

void save_store(TamagotchiApp* app) {
    // Snapshot under the mutex, then release before touching storage.
    SaveBlob blob = {.magic = SAVE_MAGIC, .version = SAVE_VERSION};
    furi_mutex_acquire(app->mutex, FuriWaitForever);
    blob.pet = app->pet;
    blob.settings = app->settings;
    furi_mutex_release(app->mutex);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, STORAGE_APP_DATA_PATH_PREFIX);
    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, &blob, sizeof(blob));
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
