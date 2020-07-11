#include "SOP_BlahBlah.h"
#include <PRM/PRM_TemplateBuilder.h>
#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <UT/UT_DSOVersion.h>
#include <stdint.h>
#include <vector>


#pragma pack(push, 1)

struct Voxel
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t i;
};

struct Header {
    char    magic[4];
    int32_t version;
};

struct Chunk {
    char    id[4];
    int32_t nContentBytes;
    int32_t childChunkLength;
    uint8_t const* content;
};

struct PACKChunk {
    int32_t nModels;
};

struct SIZEChunk {
    int32_t x, y, z;
};

struct XYZIChunk {
    int32_t nVoxels;
    Voxel const* voxels;
};

struct RGBAChunk {
    uint32_t rgba[256];
};

struct MATTChunk {
    int32_t id;
    int32_t type;
    float   weight;
    union {
        uint32_t propetryBits;
        struct {
            bool plastic : 1;
            bool roughness : 1;
            bool specular : 1;
            bool ior : 1;
            bool attenuation : 1;
            bool power : 1;
            bool glow : 1;
            bool isTotalPower : 1;
        };
    };
    float const* normPropertyValues;
};
#pragma pack(pop)

struct VoxModel
{
    struct Material
    {
        int id;
        int type;
        float weight;
        float plastic;
        float roughness;
        float specular;
        float ior;
        float attenuation;
        float power;
        float glow;
        bool isTotalPower;
    };
    std::vector<Voxel> voxels;
    std::vector<Material> mats;
    float palette[256][4];
};


static char const* theDsFile = R"THEDSFILE(
{
    name         parameters
    parm {
        name     "voxfile"
        label    "The VOX File"
        type     file
        default  { "input.vox" }
    }
}
)THEDSFILE";

static const uint32_t default_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff,
    0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
    0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
    0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
    0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc,
    0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc,
    0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
    0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
    0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
    0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999,
    0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099,
    0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
    0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
    0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
    0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366,
    0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33,
    0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
    0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
    0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
    0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00,
    0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600,
    0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
    0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
    0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
    0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700,
    0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd,
    0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

PRM_Template* blahParams()
{
    static PRM_TemplateBuilder templ( "SOP_BlahBlah", theDsFile );
    return templ.templates();
}

void newSopOperator( OP_OperatorTable* i_table )
{
    i_table->addOperator(
        new OP_Operator(
            "blahblah", //name
            "Blah Blah", //display name
            []( OP_Network* net, const char* name, OP_Operator* op ) -> OP_Node*{
                return new SOP_BlahBlah( net, name, op );
            },          // Constructor
            blahParams(), // parameters
            0,          // min # of sources
            0,          // max # of sources
            nullptr,    // local variables
            OP_FLAG_GENERATOR // generates data
        )
    );
}

// free my return value after use!
uint8_t* readWholeFile(char const* fn, size_t& size, SOP_Node* node)
{
    size = 0;
    FILE* fp = fopen(fn, "rb");
    if (!fp) {
        node->addError(SOP_MESSAGE, "Input file does not exist");
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);
#ifdef _WIN32
    size_t fsize = (size_t)_ftelli64(fp);
#else
    size_t fsize = (size_t)ftello(fp);
#endif
    uint8_t* fbuffer = static_cast<uint8_t*>(malloc(fsize));
    if (!fbuffer) {
        node->addError(SOP_MESSAGE, "failed to alloc buffer");
        fclose(fp);
        return nullptr;
    }
    fseek(fp, 0, SEEK_SET);
    if (fsize != (size = fread(fbuffer, 1, fsize, fp))) {
        node->addError(SOP_MESSAGE, "failed to read input file");
        free(fbuffer);
        fbuffer = nullptr;
    }
    fclose(fp); fp = nullptr;
    return fbuffer;
}

void convertPalette(float palette[][4], uint32_t const ipalette[])
{
    struct RGBA { uint8_t r, g, b, a; };
    for (int i = 0; i < 254; ++i) {
        RGBA const* rgba = reinterpret_cast<RGBA const*>(ipalette + i);
        palette[i + 1][0] = rgba->r / 255.0f;
        palette[i + 1][1] = rgba->g / 255.0f;
        palette[i + 1][2] = rgba->b / 255.0f;
        palette[i + 1][3] = rgba->a / 255.0f;
    }
}

bool handleChunk(Chunk const* chunk, VoxModel& model)
{
    if (memcmp("PACK", chunk->id, 4) == 0) 
    {
        // PACKChunk const* pack = reinterpret_cast<PACKChunk const*>(chunk->content);
        // useless, pass
    } 
    else if (memcmp("SIZE", chunk->id, 4) == 0) 
    {
        // SIZEChunk const* size = reinterpret_cast<SIZEChunk const*>(chunk->content);
        // useless, pass
    } 
    else if (memcmp("XYZI", chunk->id, 4) == 0) 
    {
        XYZIChunk xyzi = *reinterpret_cast<XYZIChunk const*>(chunk->content);
        xyzi.voxels = reinterpret_cast<Voxel const*>(chunk->content + offsetof(XYZIChunk, voxels));
        model.voxels.insert(model.voxels.end(), xyzi.voxels, xyzi.voxels + xyzi.nVoxels);
    } 
    else if (memcmp("RGBA", chunk->id, 4) == 0) 
    {
        RGBAChunk const* rgba = reinterpret_cast<RGBAChunk const*>(chunk->content);
        convertPalette(model.palette, rgba->rgba);
    } 
    else if (memcmp("MATT", chunk->id, 4) == 0) 
    {
        MATTChunk matt = *reinterpret_cast<MATTChunk const*>(chunk->content);
        matt.normPropertyValues = reinterpret_cast<float const*>(chunk->content + offsetof(MATTChunk, normPropertyValues));
        VoxModel::Material mat = { 0 };
        int pidx               = 0;
        mat.id                 = matt.id;
        mat.type               = matt.type;
        mat.weight             = matt.weight;
        mat.plastic            = matt.plastic ? matt.normPropertyValues[pidx++]     : 0;
        mat.roughness          = matt.roughness ? matt.normPropertyValues[pidx++]   : 0;
        mat.specular           = matt.specular ? matt.normPropertyValues[pidx++]    : 0;
        mat.ior                = matt.ior ? matt.normPropertyValues[pidx++]         : 0;
        mat.attenuation        = matt.attenuation ? matt.normPropertyValues[pidx++] : 0;
        mat.power              = matt.power ? matt.normPropertyValues[pidx++]       : 0;
        mat.glow               = matt.glow ? matt.normPropertyValues[pidx++]        : 0;
        mat.isTotalPower       = matt.isTotalPower;
        model.mats.push_back(mat);
    } else {
        return false;
    }
    return true;
}

Chunk parseChunk(uint8_t const* cursor, uint8_t const* &newcursor, VoxModel& model)
{
    Chunk chunk = *reinterpret_cast<Chunk const*>(cursor);
    chunk.content = cursor + offsetof(Chunk, content);
    newcursor = chunk.content + chunk.nContentBytes;
    return chunk;
}

VoxModel parseVox(uint8_t const* fbuffer, size_t const fsize, SOP_Node* node)
{
    VoxModel model = { {}, {}, {0} };
    convertPalette(model.palette, default_palette);

    do {
        if (!fbuffer)
            break;
        uint8_t const* cursor = fbuffer;
        auto header = reinterpret_cast<Header const*>(cursor);
        cursor += sizeof(Header);
        if (fsize <= sizeof(Header) + sizeof(Chunk) || memcmp(header->magic, "VOX ", 4) != 0) {
            node->addError(SOP_MESSAGE, "vox file broken (not vox file?)");
            break;
        }
        if (header->version > 150)
            node->addWarning(SOP_MESSAGE, "caution, vox file version higher than supported, anything may happen");

        uint8_t const* ncursor = nullptr;
        auto mainChunk = parseChunk(cursor, ncursor, model);
        if (memcmp(mainChunk.id, "MAIN", 4) != 0) {
            node->addError(SOP_MESSAGE, "no main chunk found in vox file");
            break;
        }
        cursor = ncursor;
        int32_t nChildChunkParsed = 0;
        bool extWarned = false;
        while (cursor < fbuffer + fsize) {
            Chunk c = parseChunk(cursor, ncursor, model);
            if (!handleChunk(&c, model) && !extWarned) {
                node->addWarning(SOP_MESSAGE, "got extension block in vox file, ignored");
                extWarned = true;
            }
            cursor = ncursor;
            ++nChildChunkParsed;
        }
    } while (0);
    return model;
}


OP_ERROR SOP_BlahBlah::cookMySop( OP_Context& i_context )
{
    fpreal now = i_context.getTime();
    gdp->clearAndDestroy();
    
    UT_String voxfile;
    evalString( voxfile, "voxfile", 0, now );

    size_t fsize = 0;
    //
    // `this` for displaying warning & error messages ---------+
    //                                                         |
    //                                                         V
    uint8_t* fbuffer = readWholeFile( voxfile.c_str(), fsize, this );

    if ( fbuffer )
    {
        VoxModel model = parseVox(fbuffer, fsize, this);
        if ( model.voxels.size() > 0 )
        {
            GA_Attribute* matid_attrib = gdp->addIntTuple( GA_ATTRIB_POINT, "matid", 1 );
            GA_Attribute* Cd_attrib    = gdp->addFloatTuple( GA_ATTRIB_POINT, "Cd", 3, 
                                                             GA_Defaults( GA_STORE_REAL32, 1.0f, 1.0f, 1.0f ) );
            GA_Attribute* Alpha_attrib = gdp->addFloatTuple( GA_ATTRIB_POINT, "Alpha", 1, GA_Defaults( 1.0f ) );
            GA_RWHandleF  matid( matid_attrib );
            GA_RWHandleV3 Cd( Cd_attrib );
            GA_RWHandleF  Alpha( Alpha_attrib );

            GA_Offset ptnum = gdp->appendPointBlock( model.voxels.size() );
            for( size_t i = 0; i < model.voxels.size(); i++ ) 
            {
                 auto const& voxel = model.voxels[i];
                 float const* color = model.palette[voxel.i];
                 gdp->setPos3( ptnum + i, voxel.y, voxel.z, voxel.x );
                 matid.set( ptnum + i, voxel.i );
                 Cd.set( ptnum + i, { color[0], color[1], color[2] } );
                 Alpha.set( ptnum + i, color[3] );
            }
        }

        free( fbuffer );
    }
    return error();
}