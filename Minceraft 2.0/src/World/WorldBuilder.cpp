#include "WorldBuilder.h"

namespace WorldBuilder {

    static FastNoise::SmartNode<FastNoise::OpenSimplex2> height_source;
    static FastNoise::SmartNode<FastNoise::FractalFBm> height_noise;
    static FastNoise::SmartNode<FastNoise::OpenSimplex2> cave_source;
    static FastNoise::SmartNode<FastNoise::FractalFBm> cave_noise;
    static int seed;
    static robin_hood::unordered_map<glm::ivec3, std::vector<std::shared_ptr<Structure>>> structures;

    bool init() {
        height_source = FastNoise::New<FastNoise::OpenSimplex2>();
        height_noise = FastNoise::New<FastNoise::FractalFBm>();
        height_noise->SetSource(height_source);
        height_noise->SetOctaveCount(4);

        cave_source = FastNoise::New<FastNoise::OpenSimplex2>();
        cave_noise = FastNoise::New<FastNoise::FractalFBm>();
        cave_noise->SetSource(cave_source);
        cave_noise->SetOctaveCount(1);

        return true;
    }

    float* getHeightMap(int chunk_x, int chunk_z) {
        float* height_map = new float[pc::c_length * pc::c_width];
        height_noise->GenUniformGrid2D(height_map, chunk_x * pc::c_length, chunk_z * pc::c_width, pc::c_length, pc::c_width, 0.005, seed);

        return height_map;
    }

    float* getCaveMap(int chunk_x, int chunk_y, int chunk_z) {
        float* cave_map = new float[pc::c_length * pc::c_height * pc::c_width];
        cave_noise->GenUniformGrid3D(cave_map, chunk_x * 32, chunk_y * 32, chunk_z * 32, 32, 32, 32, 0.01, seed);

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
        //float* cave_map = getCaveMap(chunk->pos.x, chunk->pos.y, chunk->pos.z);
        for (int x = 0; x < pc::c_length; x++) {
            for (int z = 0; z < pc::c_width; z++) {
                int height = floor(height_map[z * pc::c_length + x] * 50.0f);
                for (int y = 0; y < pc::c_height; y++) {
                    if ((chunk->pos.y * pc::c_height + y) == height) {
                        chunk->blocks[x][y][z].id = 1;
                        if (rand() % 256 == 1) {
                            buildTree(chunk, chunk->pos * glm::ivec3(pc::c_length, pc::c_height, pc::c_width) 
                                + glm::ivec3(x, y + 1, z));
                        }
                    }
                    else if ((chunk->pos.y * pc::c_height + y) > height - 4 &&
                        (chunk->pos.y * pc::c_height + y) < height) {
                        chunk->blocks[x][y][z].id = 2;
                    }
                    else if ((chunk->pos.y * pc::c_height + y) <= height - 4) {
                        chunk->blocks[x][y][z].id = 3;
                    }
                    /*
                    if (cave_map[(z * pc::c_length * pc::c_height) + (y * pc::c_length) + x] > 0.5f) {
                        chunk->blocks[x][y][z].id = 0;
                    }
                    */
                }
            }
        }
        
        delete height_map;
        //delete cave_map;
    }

    void buildChunk(std::shared_ptr<Chunk> chunk) {
        buildChunkTerrain(chunk);
        setStructures(chunk);
        chunk->state = Chunk::State::GENERATED;
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