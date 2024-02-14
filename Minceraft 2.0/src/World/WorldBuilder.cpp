#include "WorldBuilder.h"

namespace WorldBuilder {

    static FastNoise::SmartNode<> height_noise;
    static FastNoise::SmartNode<> cave_noise;
    static int seed;
    static robin_hood::unordered_map<glm::ivec3, std::vector<std::shared_ptr<Structure>>> structures;

    bool init() {
        height_noise = FastNoise::NewFromEncodedNodeTree("DQAFAAAAAAAAQAcAAAAAAD8AAAAAAA==");
        cave_noise = FastNoise::NewFromEncodedNodeTree("DQADAAAAAAAAQAgAAAAAAD8AAAAAAA==");
        

        return true;
    }

    float* getHeightMap(int chunk_x, int chunk_z) {
        float* height_map = new float[pc::c_length * pc::c_width];
        height_noise->GenUniformGrid2D(height_map, chunk_x * pc::c_length, chunk_z * pc::c_width, pc::c_length, pc::c_width, 0.005, seed);

        return height_map;
    }

    float* getCaveMap(int chunk_x, int chunk_y, int chunk_z) {
        float* cave_map = new float[pc::c_length * pc::c_height * pc::c_width];
        cave_noise->GenUniformGrid3D(cave_map, chunk_x * pc::c_length, chunk_y * pc::c_height, chunk_z * pc::c_width, pc::c_length, pc::c_height, pc::c_width, 0.01, seed);

        return cave_map;
    }

    std::array<std::array<float, pc::c_width>, pc::c_length> getBiomeMap(int chunk_x, int chunk_z) {
        std::array<std::array<float, pc::c_width>, pc::c_length> height_map = std::array<std::array<float, pc::c_width>, pc::c_length>();

        return height_map;
    }

    void addStructure(glm::ivec3 chunk_pos, std::shared_ptr<Structure> structure) {
        auto chunk_overlaps = structure->getChunkOverlaps();
        for (int i = 0; i < chunk_overlaps.size(); i++) {
            if (!structures.contains(chunk_pos)) {
                structures.emplace(chunk_pos, std::vector<std::shared_ptr<Structure>>());
            }
            structures[chunk_pos].push_back(structure);
            break;
        }
    }

    void buildTree(std::shared_ptr<Chunk> chunk, glm::ivec3 start) {
        // TODO: change to affect adjacent chunks
        if (start.y >= pc::c_height) {
            return;
        }
        //std::shared_ptr<Structure> tree = std::make_shared<Structure>();
        Structure tree;
        tree.setRandomTree(start, seed);
        //addStructure(chunk->pos, tree);
        tree.place(chunk);
    }

    void setStructures(std::shared_ptr<Chunk> chunk) {
        if (structures.contains(chunk->pos)) {
            for (auto& structure : structures[chunk->pos]) {
                structure->place(chunk);
                structure->addChunkCount();
                // TODO: send to chunk manager
            }
        }
    }

    void buildChunkTerrain(std::shared_ptr<Chunk> chunk) {
        float* height_map = getHeightMap(chunk->pos.x, chunk->pos.z);
        const float height_factor = 150.0f;
        float* cave_map = getCaveMap(chunk->pos.x, chunk->pos.y, chunk->pos.z);
        // TODO: use these eventually
        float cave_size = 500.0f;
        float cave_height = 500.0f;
        float cave_log_squishy = 10.0f;
        int max_cave_height = floor(cave_height / cave_log_squishy);
        for (int x = 0; x < pc::c_length; x++) {
            for (int z = 0; z < pc::c_width; z++) {
                int height = floor(height_map[z * pc::c_length + x] * height_factor);
                //height = 0;
                for (int y = 0; y < pc::c_height; y++) {
                    int absolute_height = chunk->pos.y * pc::c_height + y;
                    float cave_noise_threshhold = 500.0f / (500.0f - 10.0f * absolute_height);
                    
                    if (cave_noise_threshhold < -0.9f)
                        cave_noise_threshhold = -0.9f;
                    else if (cave_noise_threshhold > 1.0f)
                        cave_noise_threshhold = 1.0f;
                    if (absolute_height == height) {
                        chunk->blocks[x][y][z].id = 1;
                        if (rand() % 256 == 1 && absolute_height > 20) {
                            buildTree(chunk, chunk->pos * glm::ivec3(pc::c_length, pc::c_height, pc::c_width) 
                                + glm::ivec3(x, y + 1, z));
                        }
                    }
                    else if (absolute_height > height - 4 &&
                        (chunk->pos.y * pc::c_height + y) < height) {
                        chunk->blocks[x][y][z].id = 2;
                    }
                    else if (absolute_height <= height - 4) {
                        chunk->blocks[x][y][z].id = 3;
                    }
                    if (cave_map[(z * pc::c_length * pc::c_height) + (y * pc::c_length) + x] > cave_noise_threshhold &&
                        absolute_height < max_cave_height)
                    {
                        chunk->blocks[x][y][z].id = 0;
                    }
                    if (chunk->blocks[x][y][z].id == 0 && absolute_height >= -50 && absolute_height <= 0) {
                        chunk->blocks[x][y][z].id = 6;
                    }
                    // test boundaries
                    if ((x == 0 && y == 0) || (x == 0 && z == 0) || (y == 0 && z == 0) ||
                        (x == 0 && y == 31) || (x == 0 && z == 31) || (y == 0 && z == 31) || 
                        (x == 31 && y == 0) || (x == 31 && z == 0) || (y == 31 && z == 0) ||
                        (x == 31 && y == 31) || (x == 31 && z == 31) || (y == 31 && z == 31))
                    {
                        //chunk->blocks[x][y][z].id = 9;
                    }
                }
            }
        }
        
        delete height_map;
        delete cave_map;
    }

    void buildChunk(std::shared_ptr<Chunk> chunk) {
        buildChunkTerrain(chunk);
        //setStructures(chunk);
    }

    void removeChunkStructPair(glm::ivec3 chunk_pos) {
        if (structures.contains(chunk_pos)) {
            auto& chunk_structures = structures[chunk_pos];
            for (int i = 0; i < chunk_structures.size();) {
                if (chunk_structures[i]->decChunkCount()) {
                    chunk_structures.erase(chunk_structures.begin() + i);
                    // TODO: remove from other map entries

                }
                
            }
            structures.erase(chunk_pos);
        }
    }

    void setSeed(int seed) {
        WorldBuilder::seed = seed;
    }
}