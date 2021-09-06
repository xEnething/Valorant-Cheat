#include "Draw.h"
#include <iostream>

#include <algorithm>
#include <intrin.h>
#include <vector>
#include <string>
using namespace std;


#define j_junk() UsefulFuntion(25415, 75242, 2451, 7897);
inline int UsefulFuntion(int x, int y, int z, int r)
{
    for (int i = 2; i < 8; i++)
    {
        z += 456;
        r -= 55;
        x++;
        y--;

    }
    for (int i = 0; i < 2; i++)
    {
        x++;
        y += 234;
        z -= 23;
        r += 634;
    }
    return x + z + r + y;
}

bool SHOW = true;

LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
RECT GameRect = { NULL };
HWND GameWnd = NULL;
float ScreenCenterX;
float ScreenCenterY;
HWND MyWnd = NULL;
const MARGINS Margin = { -1 ,-1, -1, -1 };
MSG Message = { NULL };

#define M_PI                       3.14159265358979323846f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

struct FVector
{
    float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

    inline FVector()
        : X(0), Y(0), Z(0) {
    }

    inline FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

    __forceinline FVector operator-(const FVector& V) {
        return FVector(X - V.X, Y - V.Y, Z - V.Z);
    }

    __forceinline FVector operator+(const FVector& V) {
        return FVector(X + V.X, Y + V.Y, Z + V.Z);
    }

    __forceinline FVector operator*(float Scale) const {
        return FVector(X * Scale, Y * Scale, Z * Scale);
    }

    __forceinline FVector operator/(float Scale) const {
        const float RScale = 1.f / Scale;
        return FVector(X * RScale, Y * RScale, Z * RScale);
    }

    __forceinline FVector operator+(float A) const {
        return FVector(X + A, Y + A, Z + A);
    }

    __forceinline FVector operator-(float A) const {
        return FVector(X - A, Y - A, Z - A);
    }

    __forceinline FVector operator*(const FVector& V) const {
        return FVector(X * V.X, Y * V.Y, Z * V.Z);
    }

    __forceinline FVector operator/(const FVector& V) const {
        return FVector(X / V.X, Y / V.Y, Z / V.Z);
    }

    __forceinline float operator|(const FVector& V) const {
        return X * V.X + Y * V.Y + Z * V.Z;
    }

    __forceinline float operator^(const FVector& V) const {
        return X * V.Y - Y * V.X - Z * V.Z;
    }

    __forceinline FVector& operator+=(const FVector& v) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        return *this;
    }

    __forceinline FVector& operator-=(const FVector& v) {
        X -= v.X;
        Y -= v.Y;
        Z -= v.Z;
        return *this;
    }

    __forceinline FVector& operator*=(const FVector& v) {
        X *= v.X;
        Y *= v.Y;
        Z *= v.Z;
        return *this;
    }

    __forceinline FVector& operator/=(const FVector& v) {
        X /= v.X;
        Y /= v.Y;
        Z /= v.Z;
        return *this;
    }

    __forceinline bool operator==(const FVector& src) const {
        return (src.X == X) && (src.Y == Y) && (src.Z == Z);
    }

    __forceinline bool operator!=(const FVector& src) const {
        return (src.X != X) || (src.Y != Y) || (src.Z != Z);
    }

    __forceinline float Size() const {
        return sqrt(X * X + Y * Y + Z * Z);
    }

    __forceinline float Distance(const FVector& v)
    {

        return float(sqrtf(powf(v.X - X, 2.0) + powf(v.Y - Y, 2.0) + powf(v.Z - Z, 2.0)));
    }

    __forceinline float Size2D() const {
        return sqrt(X * X + Y * Y);
    }

    __forceinline float SizeSquared() const {
        return X * X + Y * Y + Z * Z;
    }

    __forceinline float SizeSquared2D() const {
        return X * X + Y * Y;
    }

    __forceinline float Dot(const FVector& vOther) const {
        const FVector& a = *this;
        return (a.X * vOther.X + a.Y * vOther.Y + a.Z * vOther.Z);
    }

    __forceinline FVector Normalize() {
        FVector vector;
        float length = this->Size();

        if (length != 0) {
            vector.X = X / length;
            vector.Y = Y / length;
            vector.Z = Z / length;
        }
        else
            vector.X = vector.Y = 0.0f;
        vector.Z = 1.0f;

        return vector;
    }

    __forceinline FVector ToRotator() {
        FVector rotator;
        rotator.X = -(float)atan(Z / Size2D()) * (float)(180.0f / M_PI);
        rotator.Y = (float)atan(Y / X) * (float)(180.0f / M_PI);
        rotator.Z = (float)0.f;
        if (X >= 0.f)
            rotator.Y += 180.0f;
        return rotator;
    }

    __forceinline FVector Clamp()
    {
        FVector clamped;
        while (clamped.Y < -180.0f)
            clamped.Y += 360.0f;
        while (clamped.Y > 180.0f)
            clamped.Y -= 360.0f;

        if (clamped.X < -74.0f)
            clamped.X = -74.0f;
        if (clamped.X > 74.0f)
            clamped.X = 74.0f;
        return clamped;
    }
};

struct color {
    int a, r, g, b;
    color() = default;

    color(const int r, const int g, const int b, const int a = 255) : _color{ } {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    explicit color(const uint32_t color) : _color{ } {
        this->a = color >> 24 & 0xff;
        this->r = color >> 16 & 0xff;
        this->g = color >> 8 & 0xff;
        this->b = color & 0xff;
    }

    static color from_uint(const uint32_t uint) {
        return color(uint);
    }

    unsigned char& operator[ ](const int index) {
        return _color[index];
    }

    template <typename T>
    color hsv_to_rgb(T h, T s, T v, const float alpha) const {
        int _r, _g, _b;

        if (s != 0) {

            h == 360 ? h = 0 : h = h / 60;
            auto i = static_cast<int>(trunc(h));
            int f = h - i;

            const int p = v * (1 - s);
            const int q = v * (1 - s * f);
            const int t = v * (1 - s * (1 - f));

            switch (i) {
            case 0:
                _r = v;
                _g = t;
                _b = p;
                break;

            case 1:
                _r = q;
                _g = v;
                _b = p;
                break;

            case 2:
                _r = p;
                _g = v;
                _b = t;
                break;

            case 3:
                _r = p;
                _g = q;
                _b = v;
                break;

            case 4:
                _r = t;
                _g = p;
                _b = v;
                break;

            default:
                _r = v;
                _g = p;
                _b = q;
                break;
            }
        }
        else {
            _r = v;
            _g = v;
            _b = v;
        }

        return color(static_cast<uint8_t>(_r * 255), static_cast<uint8_t>(_g * 255),
            static_cast<uint8_t>(_b * 255), alpha);
    }

    unsigned char _color[4];
};

void DrawBorder(int x, int y, int w, int h, int px, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice)
{

    DrawBox(x, (y + h - px), w, px, BorderColor, pDevice);
    DrawBox(x, y, px, h, BorderColor, pDevice);
    DrawBox(x, y, w, px, BorderColor, pDevice);
    DrawBox((x + w - px), y, px, h, BorderColor, pDevice);

}

void DrawESPBox(FVector Head, FVector Foot, DWORD dwColor, int rectAlpha, LPDIRECT3DDEVICE9 pDevice)
{
    FVector Box = Head - Foot;

    if (Box.Y < 0)
        Box.Y *= -1;

    int BoxWidth = (int)Box.Y / 2;
    int BoxHeight = (int)Box.Y / 2;
    int DrawX = (int)Head.X - (BoxWidth / 2);
    int DrawY = (int)Head.Y;

    DrawBorder(DrawX, DrawY, BoxWidth, (int)Box.Y, 1, dwColor, pDevice);
}

template <typename T>
T read(uintptr_t address)
{
    T buffer{ };
    ReadProcessMemory(GetCurrentProcess(), (LPVOID)address, &buffer, sizeof(T), 0);
    return buffer;
}

template<typename T>
bool write(uintptr_t address, T* buffer, SIZE_T sizeh)
{
    if (address > 0x7FFFFFFFFFFF || address < 1) return 0;
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, buffer, sizeh, 0);
}

__forceinline __int64 decrypt_uworld(const uint32_t key, const uintptr_t* state)
{
    unsigned __int64 v19; // r11
    unsigned __int64 v20; // r8
    unsigned __int64 v21; // r9
    unsigned int v22; // er10
    unsigned __int64 v23; // rcx
    unsigned __int64 v24; // rdx
    unsigned __int64 v25; // rcx
    int v26; // ebx
    unsigned int v27; // ecx
    __int64 v28; // rax
    unsigned __int64 v29; // r8
    unsigned __int64 v30; // r8
    unsigned __int64 v31; // rcx
    unsigned __int64 v32; // rdx
    unsigned __int64 v33; // rcx

    v19 = 2685821657736338717i64 * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12)) % 7 % 7;
    v20 = state[v19];
    v21 = (2685821657736338717i64 * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))) >> 32;
    v22 = (unsigned int)v19;
    if (!v19)
    {
        v23 = (2 * (v20 - (unsigned int)(v21 - 1))) ^ ((2 * (v20 - (unsigned int)(v21 - 1))) ^ ((v20 - (unsigned int)(v21 - 1)) >> 1)) & 0x5555555555555555i64;
        v24 = (4 * v23) ^ ((4 * v23) ^ (v23 >> 2)) & 0x3333333333333333i64;
        v25 = (16 * v24) ^ ((16 * v24) ^ (v24 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = rol8((v25 << 8) ^ ((v25 << 8) ^ (v25 >> 8)) & 0xFF00FF00FF00FFi64, 32);
    LABEL_26:
        v26 = 2 * v19;
        goto LABEL_27;
    }
    if (v22 != 1)
        goto LABEL_26;
    v26 = 2 * v19;
    v20 = rol8(v20 - (unsigned int)(2 * v19 + v21), (unsigned __int8)(((int)v21 + (int)v19) % 0x3Fu) + 1);
LABEL_27:
    v27 = v26 + v21;
    if (v22 == 2)
        v20 = ~(v20 - v27);
    switch (v22)
    {
    case 3u:
        v28 = 2 * ((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64);
        v20 = v28 ^ (v28 ^ (((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64) >> 1)) & 0x5555555555555555i64;
        break;
    case 4u:
        v29 = ror8(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v20 = (2 * v29) ^ ((2 * v29) ^ (v29 >> 1)) & 0x5555555555555555i64;
        break;
    case 5u:
        v30 = ror8(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v31 = (2 * v30) ^ ((2 * v30) ^ (v30 >> 1)) & 0x5555555555555555i64;
        v32 = (4 * v31) ^ ((4 * v31) ^ (v31 >> 2)) & 0x3333333333333333i64;
        v33 = (16 * v32) ^ ((16 * v32) ^ (v32 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = rol8((v33 << 8) ^ ((v33 << 8) ^ (v33 >> 8)) & 0xFF00FF00FF00FFi64, 32);
        break;
    case 6u:
        v20 = ~v20 - (unsigned int)(v21 + v19);
        break;
    }
    return v20 ^ (unsigned int)key;
}

uint64_t valBase = (uintptr_t)GetModuleHandleA(0);

uint64_t UWorld = 0;

uint64_t ReadWorld()
{
    uint64_t key = read<uint64_t>(valBase + 0x75CF2B8);
#pragma pack(push, 1)
    struct State
    {
        uint64_t Keys[7];
    };
#pragma pack(pop)
    const auto state = read<State>(valBase + 0x75CF280);

    return read<uint64_t>(decrypt_uworld(key, (uint64_t*)&state));
}

struct alignas(16) FQuat
{
    float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

struct alignas(16) FPlane : public FVector
{
    float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

struct FMatrix
{
    struct FPlane                                      XPlane;                                                   // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      YPlane;                                                   // 0x0010(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      ZPlane;                                                   // 0x0020(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      WPlane;                                                   // 0x0030(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)

    FMatrix operator*(const FMatrix& pM2)
    {
        FMatrix pOut;
        pOut.XPlane.X = XPlane.X * pM2.XPlane.X + XPlane.Y * pM2.YPlane.X + XPlane.Z * pM2.ZPlane.X + XPlane.W * pM2.WPlane.X;
        pOut.XPlane.Y = XPlane.X * pM2.XPlane.Y + XPlane.Y * pM2.YPlane.Y + XPlane.Z * pM2.ZPlane.Y + XPlane.W * pM2.WPlane.Y;
        pOut.XPlane.Z = XPlane.X * pM2.XPlane.Z + XPlane.Y * pM2.YPlane.Z + XPlane.Z * pM2.ZPlane.Z + XPlane.W * pM2.WPlane.Z;
        pOut.XPlane.W = XPlane.X * pM2.XPlane.W + XPlane.Y * pM2.YPlane.W + XPlane.Z * pM2.ZPlane.W + XPlane.W * pM2.WPlane.W;
        pOut.YPlane.X = YPlane.X * pM2.XPlane.X + YPlane.Y * pM2.YPlane.X + YPlane.Z * pM2.ZPlane.X + YPlane.W * pM2.WPlane.X;
        pOut.YPlane.Y = YPlane.X * pM2.XPlane.Y + YPlane.Y * pM2.YPlane.Y + YPlane.Z * pM2.ZPlane.Y + YPlane.W * pM2.WPlane.Y;
        pOut.YPlane.Z = YPlane.X * pM2.XPlane.Z + YPlane.Y * pM2.YPlane.Z + YPlane.Z * pM2.ZPlane.Z + YPlane.W * pM2.WPlane.Z;
        pOut.YPlane.W = YPlane.X * pM2.XPlane.W + YPlane.Y * pM2.YPlane.W + YPlane.Z * pM2.ZPlane.W + YPlane.W * pM2.WPlane.W;
        pOut.ZPlane.X = ZPlane.X * pM2.XPlane.X + ZPlane.Y * pM2.YPlane.X + ZPlane.Z * pM2.ZPlane.X + ZPlane.W * pM2.WPlane.X;
        pOut.ZPlane.Y = ZPlane.X * pM2.XPlane.Y + ZPlane.Y * pM2.YPlane.Y + ZPlane.Z * pM2.ZPlane.Y + ZPlane.W * pM2.WPlane.Y;
        pOut.ZPlane.Z = ZPlane.X * pM2.XPlane.Z + ZPlane.Y * pM2.YPlane.Z + ZPlane.Z * pM2.ZPlane.Z + ZPlane.W * pM2.WPlane.Z;
        pOut.ZPlane.W = ZPlane.X * pM2.XPlane.W + ZPlane.Y * pM2.YPlane.W + ZPlane.Z * pM2.ZPlane.W + ZPlane.W * pM2.WPlane.W;
        pOut.WPlane.X = WPlane.X * pM2.XPlane.X + WPlane.Y * pM2.YPlane.X + WPlane.Z * pM2.ZPlane.X + WPlane.W * pM2.WPlane.X;
        pOut.WPlane.Y = WPlane.X * pM2.XPlane.Y + WPlane.Y * pM2.YPlane.Y + WPlane.Z * pM2.ZPlane.Y + WPlane.W * pM2.WPlane.Y;
        pOut.WPlane.Z = WPlane.X * pM2.XPlane.Z + WPlane.Y * pM2.YPlane.Z + WPlane.Z * pM2.ZPlane.Z + WPlane.W * pM2.WPlane.Z;
        pOut.WPlane.W = WPlane.X * pM2.XPlane.W + WPlane.Y * pM2.YPlane.W + WPlane.Z * pM2.ZPlane.W + WPlane.W * pM2.WPlane.W;
        return pOut;
    }
};

struct alignas(16) FTransform
{
    struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FVector                                     Translation;                                              // 0x0010(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
    struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET

    FMatrix ToMatrixWithScale()
    {
        FMatrix m;
        m.WPlane.X = Translation.X;
        m.WPlane.Y = Translation.Y;
        m.WPlane.Z = Translation.Z;

        float x2 = Rotation.X + Rotation.X;
        float y2 = Rotation.Y + Rotation.Y;
        float z2 = Rotation.Z + Rotation.Z;

        float xx2 = Rotation.X * x2;
        float yy2 = Rotation.Y * y2;
        float zz2 = Rotation.Z * z2;
        m.XPlane.X = (1.0f - (yy2 + zz2)) * Scale3D.X;
        m.YPlane.Y = (1.0f - (xx2 + zz2)) * Scale3D.Y;
        m.ZPlane.Z = (1.0f - (xx2 + yy2)) * Scale3D.Z;

        float yz2 = Rotation.Y * z2;
        float wx2 = Rotation.W * x2;
        m.ZPlane.Y = (yz2 - wx2) * Scale3D.Z;
        m.YPlane.Z = (yz2 + wx2) * Scale3D.Y;

        float xy2 = Rotation.X * y2;
        float wz2 = Rotation.W * z2;
        m.YPlane.X = (xy2 - wz2) * Scale3D.Y;
        m.XPlane.Y = (xy2 + wz2) * Scale3D.X;

        float xz2 = Rotation.X * z2;
        float wy2 = Rotation.W * y2;
        m.ZPlane.X = (xz2 + wy2) * Scale3D.Z;
        m.XPlane.Z = (xz2 - wy2) * Scale3D.X;

        m.XPlane.W = 0.0f;
        m.YPlane.W = 0.0f;
        m.ZPlane.W = 0.0f;
        m.WPlane.W = 1.0f;

        return m;
    }
};

class CameraStruct
{
public:
    FVector Position; //0x11B0 
    FVector Rotation; //0x11BC 
    float Fov; //0x11C8 
}; //Size=0x11CC

namespace Offsets
{
    uintptr_t oLevel = 0x38;
    uintptr_t oGameInstance = 0x1A8;
    uintptr_t oLocalPlayers = 0x40;
    uintptr_t oPlayerController = 0x38;
    uintptr_t oLocalPawn = 0x440;
    uintptr_t RootComp = 0x210;
    uintptr_t oCameraManager = 0x458;
    uintptr_t oCameraCache = 0x1220;
    uintptr_t ObjID = 0x18;

    uintptr_t Dormant = 0xD8;
    uintptr_t RelativeLocation = 0x15C;
    uintptr_t RelativeRotation = 0x168;
    uintptr_t PlayerState = 0x3C8;
    uintptr_t Ping = 0x3B8;

    uintptr_t TeamComponent = 0x598;
    uintptr_t Team = 0xF8;
    uintptr_t DamageHandler = 0x968;
    uintptr_t Health = 0x188;
    uintptr_t HealthMax = 0x198;

    uintptr_t Mesh = 0x408;
    uintptr_t StaticMesh = 0x518;
    uintptr_t StaticMesh_Cached = 0x528;

    uintptr_t ComponentToWorld = 0x250;
    uintptr_t UniqueID = 0x38;

    uintptr_t ControlRotation = 0x418;

    uintptr_t PlayerName = 0x3A8;
};

uint64_t PlayerController = 0;

uint64_t CameraCache = 0;

CameraStruct GetCameraCache()
{
    CameraStruct Camera = read<CameraStruct>(CameraCache + Offsets::oCameraCache);
    return Camera;
}

D3DMATRIX to_matrix(const FVector& rotation, const FVector& origin) {

    const auto pitch = rotation.X * float(3.14159265358979323846f) / 180.f;
    const auto yaw = rotation.Y * float(3.14159265358979323846f) / 180.f;
    const auto roll = rotation.Z * float(3.14159265358979323846f) / 180.f;

    const auto SP = sinf(pitch);
    const auto CP = cosf(pitch);
    const auto SY = sinf(yaw);
    const auto CY = cosf(yaw);
    const auto SR = sinf(roll);
    const auto CR = cosf(roll);

    D3DMATRIX matrix;
    matrix._11 = CP * CY;
    matrix._12 = CP * SY;
    matrix._13 = SP;
    matrix._14 = 0.f;

    matrix._21 = SR * SP * CY - CR * SY;
    matrix._22 = SR * SP * SY + CR * CY;
    matrix._23 = -SR * CP;
    matrix._24 = 0.f;

    matrix._31 = -(CR * SP * CY + SR * SY);
    matrix._32 = CY * SR - CR * SP * SY;
    matrix._33 = CR * CP;
    matrix._34 = 0.f;

    matrix._41 = origin.X;
    matrix._42 = origin.Y;
    matrix._43 = origin.Z;
    matrix._44 = 1.f;

    return matrix;

}

FVector WorldToScreen(FVector& world_loc) {

    auto CamCache = GetCameraCache();
    const auto matrix = to_matrix(CamCache.Rotation, FVector());
    const auto axisx = FVector(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
    const auto axisy = FVector(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
    const auto axisz = FVector(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);

    const auto delta = world_loc - CamCache.Position;
    auto transformed = FVector(delta.Dot(axisy), delta.Dot(axisz), delta.Dot(axisx));

    if (transformed.Z < 0.001f)
        transformed.Z = 0.001f;

    const auto fov_angle = CamCache.Fov;
    const float center = Width / 2.f;
    const float centery = Height / 2.f;
    return FVector(center + transformed.X * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.Z, centery - transformed.Y * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.Z, 0.f);

}

bool WorldToScreen2(FVector& world_loc, FVector& screenLoc) {

    auto CamCache = GetCameraCache();
    const auto matrix = to_matrix(CamCache.Rotation, FVector());
    const auto axisx = FVector(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
    const auto axisy = FVector(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
    const auto axisz = FVector(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);

    const auto delta = world_loc - CamCache.Position;
    auto transformed = FVector(delta.Dot(axisy), delta.Dot(axisz), delta.Dot(axisx));

    if (transformed.Z < 0.001f)
        transformed.Z = 0.001f;

    const auto fov_angle = CamCache.Fov;
    const float screenCenterX = Width / 2.f;
    const float screenCenterY = Height / 2.f;

    screenLoc.X = screenCenterX + transformed.X * (screenCenterX / (float)tan(fov_angle * M_PI / 360)) / transformed.Z;
    screenLoc.Y = screenCenterY - transformed.Y * (screenCenterX / (float)tan(fov_angle * M_PI / 360)) / transformed.Z;

    return ((screenLoc.X >= 0.0f) && (screenLoc.X <= Width) && (screenLoc.Y >= 0.0f) && (screenLoc.Y <= Height));
}


#define URotationToRadians(URotation)		((URotation)* (M_PI / 32768.0f))
#define URotationToDegree( URotation )		( ( URotation ) * ( 360.0f / 65536.0f ) ) 

#define DegreeToURotation( Degree )			( ( Degree ) * ( 65536.0f / 360.0f ) )
#define DegreeToRadian( Degree )			( ( Degree ) * ( M_PI / 180.0f ) )

#define RadianToURotation( URotation )		( ( URotation ) * ( 32768.0f / M_PI ) ) 
#define RadianToDegree( Radian )			( ( Radian ) * ( 180.0f / M_PI ) )

#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )

bool bumbum = true;
__forceinline void Clamp(FVector& Ang) {
    if (Ang.X < 0.f)
        Ang.X += 360.f;

    if (Ang.X > 360.f)
        Ang.X -= 360.f;

    if (Ang.Y < 0.f) Ang.Y += 360.f;
    if (Ang.Y > 360.f) Ang.Y -= 360.f;
    Ang.Z = 0.f;
}

__forceinline FVector CalcAngle(FVector Src, FVector& Dst) {
    FVector Delta = Src - Dst;
    const auto sqrtss = [](float in) {
        __m128 reg = _mm_load_ss(&in);
        return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
    };

    float hyp = sqrtss(Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z);

    FVector Rotation{};
    Rotation.X = RadianToDegree(acosf(Delta.Z / hyp));
    Rotation.Y = RadianToDegree(atanf(Delta.Y / Delta.X));
    Rotation.Z = 0;
    if (Delta.X >= 0.0f) Rotation.Y += 180.0f;
    Rotation.X += 270.f;
    return Rotation;
}


//pcontroller + ProjectW2S = 0x828
void ClientSetRotation(FVector NewRotation)
{
    auto ClientSetRotationAddr = read<uintptr_t>(PlayerController) + 0x658;
    auto fClientSetRotation = *((void(__fastcall**)(uintptr_t AController, FVector NewRotation, bool bResetCamera))(ClientSetRotationAddr));
    fClientSetRotation(PlayerController, NewRotation, true);
}

bool is_visible(uint64_t mesh)
{
    bool bVisible;

    float fLastSubmitTime = read<float>(mesh + 0x340);
    float fLastRenderTimeOnScreen = read<float>(mesh + 0x340 + 8);
    const float fVisionTick = 0.06f;
    bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
    return bVisible;
}
/*
48 8B C4 55 56 57 41 56 41 57 48 8D 68 C8 > LineOfSightTo
48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 41 0F B6 E9 > WorldToScreen
48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC ?? ?? ?? ?? F6 81 > GetBoneMatrix
*/
#define line_of_sight 0x37A2210//0x357F248//0x357F250
#define get_bone_matrix 0x357DDD0
#define project_w2s 0x389FB40

FTransform GetBoneIndex(uintptr_t mesh, int index)
{
    uintptr_t bonearray = read<uintptr_t>(mesh + Offsets::StaticMesh);
    if (bonearray == 0)
        bonearray = read<uintptr_t>(mesh + Offsets::StaticMesh_Cached);

    return read<FTransform>(bonearray + (index * 0x30));
}

FVector GetBoneWithRotation(uintptr_t mesh, int id)
{
    FTransform Bone = GetBoneIndex(mesh, id);
    if (Bone.Scale3D.Z > 0.f && Bone.Scale3D.Y > 0.f && Bone.Scale3D.Z > 0.f)
    {
        FTransform ComponentToWorld = read<FTransform>(mesh + Offsets::ComponentToWorld);
        FMatrix Matrix = Bone.ToMatrixWithScale() * ComponentToWorld.ToMatrixWithScale();
        return FVector{ Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z };
    }
}

void DrawSkeleton(uintptr_t Mesh, bool IsMale, DWORD col)
{
    __try
    {
        int skeleton[][4] = {
            { 7, 8, 0, 0},
            { 7, IsMale ? 47 : 46, IsMale ? 48 : 47, IsMale ? 49 : 48 },
            { 7, 21, 22, 23 },
            { 7, 5, 4, 3 },
            { 3, IsMale ? 82 : 80, IsMale ? 83 : 81, IsMale ? 85 : 83 },
            { 3, IsMale ? 75 : 73, IsMale ? 76 : 74, IsMale ? 78 : 76 }
        };
        for (auto part : skeleton)
        {
            FVector previous{};
            for (int i = 0; i < 4; i++)
            {
                if (!part[i]) break;

                FVector current = GetBoneWithRotation(Mesh, part[i]);
                if (previous.X == 0.f)
                {
                    previous = current;
                    continue;
                }
                FVector p1{}, c1{};
                p1 = WorldToScreen(previous);
                c1 = WorldToScreen(current);
                DrawLine(p1.X, p1.Y, c1.X, c1.Y, col, 1.0f);
                previous = current;
            }
        }
    }
    __except (1) {}
}


bool isaimbotting;
//#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))
/*
bool isVisible(uintptr_t de_mesh)
{
    bool bVisible;
    float fLastSubmitTime = read<float>(de_mesh + 0x0340);
    float fLastRenderTimeOnScreen = read<float>(de_mesh + 0x0344);
    const float fVisionTick = 0.04f;
    bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
    return bVisible;
}*/

uintptr_t ULevel;
uintptr_t GameInstance;
uintptr_t LocalPlayer;
uintptr_t AcknowledgedPawn;
uintptr_t LocalPlayerState;

uintptr_t entityx = 0;

float DistanceBetweenCross(float X, float Y)
{
    float ydist = (Y - (ScreenCenterY));
    float xdist = (X - (ScreenCenterX));
    float Hypotenuse = sqrt(pow(ydist, 2) + pow(xdist, 2));
    return Hypotenuse;
}

void Circle(int X, int Y, int radius, int numSides, DWORD Color)
{
    D3DXVECTOR2 Line[128];
    float Step = M_PI * 2.0 / numSides;
    int Count = 0;
    for (float a = 0; a < M_PI * 2.0; a += Step)
    {
        float X1 = radius * cos(a) + X;
        float Y1 = radius * sin(a) + Y;
        float X2 = radius * cos(a + Step) + X;
        float Y2 = radius * sin(a + Step) + Y;
        Line[Count].x = X1;
        Line[Count].y = Y1;
        Line[Count + 1].x = X2;
        Line[Count + 1].y = Y2;
        Count += 2;
    }
    S_Line->Begin();
    S_Line->Draw(Line, Count, Color);
    S_Line->End();
}

struct D3DTLVERTEX
{
    float fX;
    float fY;
    float fZ;
    float fRHW;
    D3DCOLOR Color;
    float fU;
    float fV;
};


D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V)
{
    D3DTLVERTEX v =
    {
        X,
        Y,
        Z,
        RHW,
        color,
        U,
        V
    };

    return v;
}

void DrawFilledCircle(int xPos, int yPos, int Radius, D3DCOLOR color)
{
    const DWORD D3DFVF_TL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    D3DTLVERTEX Vtex[400];

    float x1 = xPos;
    float y1 = yPos;

    for (int i = 0; i <= 363; i += 3)
    {
        float angle = (i / 56.3f);
        float x2 = xPos + (Radius * sin(angle));
        float y2 = yPos + (Radius * cos(angle));
        Vtex[i] = CreateD3DTLVERTEX(xPos, yPos, 0, 1, color, 0, 0);
        Vtex[i + 1] = CreateD3DTLVERTEX(x1, y1, 0, 1, color, 0, 0);
        Vtex[i + 2] = CreateD3DTLVERTEX(x2, y2, 0, 1, color, 0, 0);

        y1 = y2;
        x1 = x2;
    }

    d3ddev->SetFVF(D3DFVF_TL);
    d3ddev->SetTexture(0, NULL);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 363, Vtex, sizeof(D3DTLVERTEX));
}
float AimFOV = 30.f;
bool GetClosestPlayerToCrossHair(FVector Pos, float& max, float aimfov)
{
    float Dist = DistanceBetweenCross(Pos.X, Pos.Y);
    if (Dist < max)
    {
        max = Dist;
        AimFOV = aimfov;
        return true;
    }
    return false;
}

void AIms(uintptr_t ent)
{
    float max = AimFOV / 2;
    FVector rootHeadOut = FVector();

    uintptr_t Mesh = read<uintptr_t>(ent + Offsets::Mesh);
    FVector rootHead = GetBoneWithRotation(Mesh, 7);
    rootHeadOut = WorldToScreen(rootHead);

    if (GetClosestPlayerToCrossHair(rootHeadOut, max, AimFOV))
        entityx = ent;
    return;
}

static bool InsideCircle(float xc, float yc, float r, float x, float y)
{

    float dx = xc - x;
    float dy = yc - y;
    return dx * dx + dy * dy <= r * r;
}

FVector LGetViewAngles()
{
    FVector ViewAngle = read<FVector>(PlayerController + Offsets::ControlRotation);
    return ViewAngle;
}

void normalize(FVector& in)
{
    if (in.X > 89.f) in.X -= 360.f;
    else if (in.X < -89.f) in.X += 360.f;

    // in.Y = fmodf(in.Y, 360.0f);
    while (in.Y > 180)in.Y -= 360;
    while (in.Y < -180)in.Y += 360;
    in.Z = 0;
}

FVector SmoothAim(FVector Camera_rotation, FVector Target, float SmoothFactor)
{
    FVector diff = Target - Camera_rotation;
    normalize(diff);
    return Camera_rotation + diff / SmoothFactor;
}


void RCS(FVector Target, FVector Camera_rotation, float SmoothFactor) {

    // Camera 2 Control space
    FVector ConvertRotation = Camera_rotation;
    normalize(ConvertRotation);

    // Calculate recoil/aimpunch
    auto ControlRotation = read<FVector>(PlayerController + Offsets::ControlRotation);
    FVector DeltaRotation = ConvertRotation - ControlRotation;
    normalize(DeltaRotation);

    // Remove aimpunch from CameraRotation
    ConvertRotation = Target - (DeltaRotation * SmoothFactor);
    normalize(ConvertRotation);

    //Smooth the whole thing
    FVector Smoothed = SmoothAim(Camera_rotation, ConvertRotation, SmoothFactor);
    Smoothed -= (DeltaRotation / SmoothFactor);
    Clamp(Smoothed);
    // normalize(Smoothed);
    *(float*)(PlayerController + 0x418) = Smoothed.X;
    *(float*)(PlayerController + 0x41C) = Smoothed.Y;

    return;
}

bool LineOfSightTo(uint64_t thiz, uint64_t Other, FVector* ViewPoint)
{
    __try {
        static DWORD_PTR funcAddress = 0;
        if (!funcAddress)funcAddress = valBase + line_of_sight;
        if (funcAddress)
        {
            return reinterpret_cast<int(__fastcall*)(uint64_t thiz, uint64_t Other, FVector * ViewPoint)>(funcAddress)(thiz, Other, ViewPoint);
        }
        else return false;
    }
    __except (1) { return false; }
}
float HealthLocal;
float Smooth = 1.f;
void xasdasdascsa()
{
    
    //(LocalPawn + 0xF28) + 0x210 = Matrix
    //(LocalPawn + 0xF28) + 0x240 = Pitch & Yaw
    bool IsDormant = read<bool>(entityx + Offsets::Dormant);
    uintptr_t Mesh = read<uintptr_t>(entityx + Offsets::Mesh);
    
    if (AcknowledgedPawn != 0)
    {
        uintptr_t DmgHandler = read<uintptr_t>(entityx + Offsets::DamageHandler);
        float Health = read<float>(DmgHandler + Offsets::Health);
        
        FVector aim_bone = GetBoneWithRotation(Mesh, (Fun.Scroll_Function1 == 0 ? 8 : Fun.Scroll_Function1 == 1 ? 7 : 5));
        FVector vHead = WorldToScreen(aim_bone);
        if (entityx != 0) {
            
            if ((Health > 0.f && Health < 200.f) && !IsDormant)
            {
                bool IsVisible = is_visible(Mesh);

                if (IsVisible)
                {
                    if (vHead.X != 0.f && vHead.Y != 0.f &&
                        aim_bone.X != 0.f && aim_bone.Y != 0.f && aim_bone.Z != 0.f)
                    {
                        if (GetAsyncKeyState(VK_LBUTTON) ?
                            InsideCircle(ScreenCenterX, ScreenCenterY, 180.f, vHead.X, vHead.Y) :
                            InsideCircle(ScreenCenterX, ScreenCenterY, AimFOV, vHead.X, vHead.Y))
                        {
                            if (DistanceBetweenCross(vHead.X, vHead.Y))
                            {
                                FVector AimAngles = CalcAngle(GetCameraCache().Position, aim_bone);
                                Clamp(AimAngles);
                                if (Fun.cRCS > 0)
                                {
                                    RCS(AimAngles, GetCameraCache().Rotation, Smooth + 3.f);
                                }
                                else
                                {
                                    if (Fun.Scroll_Function4 > 0) { // smooth açık
                                        AimAngles = SmoothAim(GetCameraCache().Rotation, AimAngles, Smooth);
                                        Clamp(AimAngles);
                                    }
                                    *(float*)(PlayerController + 0x418) = AimAngles.X;
                                    *(float*)(PlayerController + 0x41C) = AimAngles.Y;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


DWORD Aimkey = NULL;
bool GetHead()
{
    
    if (Aimkey != NULL)
        return (GetAsyncKeyState(Aimkey));
    else
        return true;
}

void Aimbot()
{
    if (entityx)
    {
        if (GetHead())
        {
            isaimbotting = true;
            xasdasdascsa();
        }
        else
        {
            isaimbotting = false;
        }
    }
    else
    {
        isaimbotting = false;
    }
    
}

void bar_hp(int x, int y, int width, int height, int hp, int Alpha)
{
    DWORD backcolor = D3DCOLOR_RGBA(0, 0, 0, 255);
    DWORD color = D3DCOLOR_RGBA(6, 220, 0, 255);

    int healthPercentage = hp * 100 / 100;
    if (healthPercentage <= 80 && healthPercentage >= 60) { color = D3DCOLOR_RGBA(255, 255, 0, Alpha); }
    else if (healthPercentage <= 60 && healthPercentage >= 40) { color = D3DCOLOR_RGBA(255, 200, 0, Alpha); }
    else if (healthPercentage <= 40 && healthPercentage >= 20) { color = D3DCOLOR_RGBA(255, 120, 0, Alpha); }
    else if (healthPercentage <= 20 && healthPercentage >= 0) { color = D3DCOLOR_RGBA(255, 24, 101, Alpha); }

    DrawBox(x, y, width, (hp * height / 100), color, d3ddev);
}

/* Male's */
/* 10475983 = Breach
 * 10486177 = Brimstone
 * 10480426 = Cypher
 * 10490019 = Omen
 * 10485211 = Phoenix
 * 10481647 = Sova
 * 10487634 = YORU
 */

 /* Female's */
 /* 10490894 = Jett
  * 10482853 = Killjoy
  * 10476549 = Raze
  * 10489239 = Reyna
  * 10488471 = Sage
  * 10479233 = Skye
  * 10484198 = Viper
  */
  //Bot = 10477073

int Aimbonz = 8;

typedef struct _ValEntity
{
    uintptr_t Addr;
    uintptr_t Mesh;
    int Type;
    int ObjectID;

}ValEntity;
vector<ValEntity> entityList;

#define Player 0
#define Bot 1
#define SpikeTimer 2

bool ol = true;



/* Ulti's */
/* Skye Wolf      = 10678150 //ok
 * Skye Bird      = 10677811 //ok
 * Skye Ult.      = 10478935
 * Sova Arrow     = 10680502 //ok
 * Cypher Camera  = 10679492 //ok
 * Cypher Trap    = 10678701 //ok
 * Killjoy Turret = 10681574 //ok
 * Killjoy Alarm Bot  = 10681975 //ok
 * Killjoy Bomb Trap  = 10681307 //ok
 * Killjoy Ulti       = 10682227 //ok
 * Raze Bot           = 10675151 //ok
*/

// Spike Esp = 10769634

string GetAbilities(int id)
{
    switch (id)
    {
    case 10678150: return ("Skye Wolf");
        break;
    case 10677811: return ("Skye Bird");
        break;
    case 10478935: return ("Skye Ulti");
        break;
    case 10680502: return ("Sova Arrow");//ok
        break;
    case 10679492: return ("Cypher Camera");//ok
        break;
    case 10678701: return ("Cypher Trap");//ok
        break;
    case 10681574: return ("Killjoy Turret");//ok
        break;
    case 10681975: return ("Killjoy Alarm Bot");//ok
        break;
    case 10681307: return ("Killjoy Bomb Trap");//ok
        break;
    case 10682227: return ("Killjoy Ulti");//ok
        break;
    case 10675151: return ("Raze Bot");//ok
        break;
    case 10686257: return ("Yoru TP");
        break;
    case 10685993: return ("Yoru FAKEOUT");
        break;
    case 10769634: return ("Spike");
        break;
    default:
        return ("NULL");
        break;
    }
}


/* Male's */
/* 10674110 = Breach ok
 * 10684347 = Brimstone ok
 * 10678577 = Cypher ok
 * 10688224 = Omen ok
 * 10683381 = Phoenix ok
 * 10679798 = Sova ok
 * 10685839 = YORU ok
 */

 /* Female's */
 /* 10689099 = Jett ok
  * 10681004 = Killjoy ok
  * 10674676 = Raze ok
  * 10687444 = Reyna ok
  * 10686676 = Sage ok
  * 10677384 = Skye ok
  * 10682349 = Viper ok
  */
  //Bot = 10477073
#define isMale (ObjID == 11371326 || ObjID == 11382997 || ObjID == 11375864 || ObjID == 11386956 || ObjID == 11380680 || ObjID == 11377095 || ObjID == 11384528)
#define isFemale (ObjID == 11381645 || ObjID == 11387840 || ObjID == 11378301 || ObjID == 11371901 || ObjID == 11386165 || ObjID == 11385388 || ObjID == 11374667 || ObjID == 11379638)

#define isBot (ObjID == 10732217)

string GetCharacterName(int id)
{

    switch (id)
    {
        /*Female's*/
    case 11381645:   return xorstr_("Astra");//ok
        break;
    case 11387840:   return xorstr_("Jett");//ok
        break;
    case 11378301:   return xorstr_("Killjoy");//ok
        break;
    case 11371901:   return xorstr_("Raze");//ok
        break;
    case 11386165:   return xorstr_("Reyna");//ok
        break;
    case 11385388:   return xorstr_("Sage");//ok
        break;
    case 11374667:   return xorstr_("Skye");//ok
        break;
    case 11379638:   return xorstr_("Viper");//ok
        break;

        /*Male's*/
    case 11371326:   return xorstr_("Breach");//ok
        break;
    case 11382997:   return xorstr_("Brimstone");//ok
        break;
    case 11375864:   return xorstr_("Cypher");//ok
        break;
    case 11386956:   return xorstr_("Omen");//ok
        break;
    case 11380680:   return xorstr_("Phoenix");//ok
        break;
    case 11377095:   return xorstr_("Sova");//ok
        break;
    case 11384528:   return xorstr_("Yoru");//ok
        break;

        /*Bot*/
    //case 10732217:  return  xorstr_("Bot");
    //    break;
    default:

        return ("");
        break;
    }
}


void cache()
{
    
    while (true)
    {
        vector<ValEntity> TempList;
        ULevel = read<uintptr_t>(UWorld + Offsets::oLevel);//page guard
        if (ULevel == NULL || GameInstance == NULL || LocalPlayer == NULL || PlayerController == NULL)
            UWorld = ReadWorld();  //page guard
        GameInstance = read<uintptr_t>(UWorld + Offsets::oGameInstance);
        if (GameInstance == 0) continue;
        LocalPlayer = read<uintptr_t>(read<uintptr_t>(GameInstance + Offsets::oLocalPlayers));
        if (LocalPlayer == 0) continue;
        PlayerController = read<uintptr_t>(LocalPlayer + Offsets::oPlayerController);
        if (PlayerController == 0) continue;
        CameraCache = read<uint64_t>(PlayerController + Offsets::oCameraManager);
        if (CameraCache == 0) continue;
        AcknowledgedPawn = read<uintptr_t>(PlayerController + Offsets::oLocalPawn);
        if (AcknowledgedPawn == 0) continue;
        uintptr_t DmgHandlerLocal = read<uintptr_t>(AcknowledgedPawn + Offsets::DamageHandler);
        if (DmgHandlerLocal == 0) continue;
        LocalPlayerState = read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState);
        if (LocalPlayerState == 0) continue;
        auto ActorArray = read<uintptr_t>(ULevel + 0xA0);
        if (ActorArray == 0) continue;
        
        HealthLocal = read<float>(DmgHandlerLocal + Offsets::Health);

        int ActorCount = read<int>(ULevel + 0xA8);
        for (int i = 0; i < ActorCount; i++)
        {
            
            uintptr_t Actor = read<uintptr_t>(ActorArray + (i * 0x8));

            if (Actor != 0)
            {
                uintptr_t Mesh = read<uintptr_t>(Actor + Offsets::Mesh);
                int UniqueId = read<int>(Actor + Offsets::UniqueID);

                ValEntity valEntity{ };
                valEntity.Addr = Actor;

                int ObjID = read<int>(Actor + Offsets::ObjID);

                if (isMale || isFemale)
                {
                    if (Mesh != NULL)
                    {
                        
                        valEntity.ObjectID = ObjID;
                        valEntity.Mesh = Mesh;
                        valEntity.Type = Player;
                        TempList.push_back(valEntity);
                    }
                }
            }
        }
        entityList = TempList;
        Sleep(2);
    }   
}


D3DXVECTOR2 WorldRadar(FVector srcPos, FVector distPos, float yaw, float radarX, float radarY, float size)
{
    
    auto cosYaw = cos(DegreeToRadian(yaw));
    auto sinYaw = sin(DegreeToRadian(yaw));

    auto deltaX = srcPos.X - distPos.X;
    auto deltaY = srcPos.Y - distPos.Y;

    auto locationX = (float)(deltaY * cosYaw - deltaX * sinYaw) / 45.f;
    auto locationY = (float)(deltaX * cosYaw + deltaY * sinYaw) / 45.f;

    if (locationX > (size - 2.f))
        locationX = (size - 2.f);
    else if (locationX < -(size - 2.f))
        locationX = -(size - 2.f);

    if (locationY > (size - 6.f))
        locationY = (size - 6.f);
    else if (locationY < -(size - 6.f))
        locationY = -(size - 6.f);

    return D3DXVECTOR2((int)(-locationX + radarX), (int)(locationY + radarY));
    
}
static FVector pRadar;

static int enemy_size = 3;


void DrawRadar(FVector EntityPos, DWORD EntityColor)
{
    auto radar_posX = pRadar.X + 135;
    auto radar_posY = pRadar.Y + 135;
    if (AcknowledgedPawn == 0) return;
    uint64_t LocalRootComp = read<uint64_t>(AcknowledgedPawn + Offsets::RootComp);
    FVector LocalPos = read<FVector>(LocalRootComp + Offsets::RelativeLocation);
    auto Radar2D = WorldRadar(LocalPos, EntityPos, LGetViewAngles().Y, radar_posX, radar_posY, 135.f);// radar pos

    DrawBox(Radar2D.x, Radar2D.y, 4, 4, EntityColor, d3ddev);
    
}

void EspLoop()
{
    UsefulFuntion(156, 155, 321, 551);
    std::vector<ValEntity> vEntityList = entityList;
    
    if (vEntityList.empty())
        return;
    
    for (int i = 0; i < vEntityList.size(); i++)
    {
        ValEntity pEntity = vEntityList[i];
        if (AcknowledgedPawn != NULL &&
            pEntity.Addr != NULL &&
            pEntity.Addr != AcknowledgedPawn)
        {
            UsefulFuntion(412, 123, 421, 512);
            uintptr_t Mesh = pEntity.Mesh;
            uintptr_t bonearray = read<uintptr_t>(Mesh + Offsets::StaticMesh);

            uintptr_t RootComp = read<uintptr_t>(pEntity.Addr + Offsets::RootComp);
            FVector RootPos = read<FVector>(RootComp + Offsets::RelativeLocation);
            FVector vScreen = WorldToScreen(RootPos);

            uintptr_t LocalRootComp = read<uintptr_t>(AcknowledgedPawn + Offsets::RootComp);
            FVector LocalPos = read<FVector>(LocalRootComp + Offsets::RelativeLocation);
            float fDistance = LocalPos.Distance(RootPos) / 100.f;

            int ObjID = pEntity.ObjectID;

            if (pEntity.Type == Player)
            {
                int unique_id = read<int>(pEntity.Addr + Offsets::UniqueID);
                if (unique_id != 18743553)
                    continue;

                uintptr_t DmgHandler = read<uintptr_t>(pEntity.Addr + Offsets::DamageHandler);

                float Health = read<float>(DmgHandler + Offsets::Health);

                if (Health > 0.f && Health < 200.f)
                {
                    char dist_char[64];
                    string CharacterName = GetCharacterName(pEntity.ObjectID);
                    sprintf(dist_char, CharacterName.c_str());

                    char dist_[64];
                    sprintf(dist_, ("%.f"), fDistance);

                    bool IsDormant = read<bool>(pEntity.Addr + Offsets::Dormant);

                    uintptr_t ePlayerState = read<uintptr_t>(pEntity.Addr + Offsets::PlayerState);

                    int EnemyTeam = read<int>(read<uintptr_t>(read<uintptr_t>(pEntity.Addr + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);
                    int LocalTeam = read<int>(read<uintptr_t>(read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);

                    if (fDistance > 1.f)
                    {
                        FVector Head = GetBoneWithRotation(Mesh, 8);
                        FVector Foot = GetBoneWithRotation(Mesh, 0);

                        FVector vHead = WorldToScreen(Head);
                        FVector vFoot = WorldToScreen(Foot);
                        float BoxHeight = vHead.Y - vFoot.Y;


                        FVector vHead2;
                        bool fuckk = WorldToScreen2(Head, vHead2);

                        FVector vFoot2;
                        bool fuckk2 = WorldToScreen2(Foot, vFoot2);

                        FVector vPoint(0, 0, 0);
                        bool IsVisible = is_visible(Mesh);

                        DWORD vis_col = IsDormant ? Silver_Menu : IsVisible ? GREEN : RED;

                        if (EnemyTeam != LocalTeam)
                        {
                           // AIms(pEntity.Addr);

                            if (Fun.cRadar == 1)
                                DrawRadar(RootPos, vis_col);

                            if (fuckk && fuckk2) 
                            {
                                if (Fun.cHealthbar == 1) {

                                    const int middle = vFoot2.Y - vHead2.Y;
                                    const auto width = middle / 4;
                                    const auto health_calc = std::clamp(static_cast<int>(Health) * middle / 100, 0, middle);
                                    const auto health_color = color().hsv_to_rgb(int(Health) + 25, 1, 1, 255);
                                    font->DrawTextX(vFoot2.X - (strlen(string(std::to_string((int)Health) + (" HP")).c_str()) * 7 / 2) + 5, vFoot2.Y + 5, IsDormant ? D3DCOLOR_RGBA(236, 236, 236, 255) : D3DCOLOR_RGBA(health_color.r, health_color.g, health_color.b, 255), string(std::to_string((int)Health) + " HP").c_str());

                                    DrawBox(vHead2.X + width, vHead2.Y + middle - health_calc, 4, health_calc, D3DCOLOR_RGBA(health_color.r, health_color.g, health_color.b, 255), d3ddev);
                                }

                                if (Fun.cBox == 1)
                                    DrawESPBox(vHead2, vFoot2, vis_col, IsDormant ? 40 : 100, d3ddev);
                            }

                            if (Fun.cCharacter == 1) {
                                DrawBox(vHead.X - (strlen(dist_char) * 7 / 2) - 27, vHead.Y - 25, (3 * 7 / 2) + 16, 16, RED, d3ddev);
                                DrawBox(vHead.X - (strlen(dist_char) * 7 / 2) - 1, vHead.Y - 25, strlen(dist_char) * 7 + 14, 16, BLACK, d3ddev);
                                font->DrawTextX(vHead.X - (strlen(dist_char) * 7 / 2) - (fDistance < 10.f ? 17 : 21), vHead.Y - 25, Silver_Menu, dist_);
                                font->DrawTextX(vHead.X - (strlen(dist_char) * 7 / 2) + 7, vHead.Y - 25, vis_col, dist_char);
                            }

                            if (Fun.cHead == 1) {
                                if (fuckk) {
                                    DrawFilledCircle(vHead2.X, vHead2.Y, BoxHeight / 8, YELLOW);
                                }
                            }
                        }
                    }
                }
            }
        }     
    }
}
#define SilverWhite2       D3DCOLOR_RGBA(200, 200, 200, 200)
#define TBlack2     D3DCOLOR_ARGB(100, 000, 000, 000)
void Menu(LPDIRECT3DDEVICE9 pDevice)
{
    
    pRadar.X = Width - 450;
    pRadar.Y = ScreenCenterY - ScreenCenterY + 300;
    
    if (Fun.cRadar == 1)
    {      
        DrawBorder(pRadar.X, pRadar.Y, 270, 270, 1, RED, d3ddev);       
        auto radar_posX = pRadar.X + 135;//155
        auto radar_posY = pRadar.Y + 135;//165
        DrawBox(pRadar.X, pRadar.Y, 270, 270, TBlack2, d3ddev);
        DrawBox(radar_posX - 0.5f, radar_posY - 0.5f, 3, 3, YELLOW, d3ddev);
    }
    

	if (GetAsyncKeyState(VK_DELETE) & 1) SHOW = (!SHOW);
	if (SHOW)
	{

        //font->DrawTextX(10, 5, GREEN, xorstr_("UC ESP ~ Public"));
		X = 0;
		Y = 0;
		Draw_Menu_1(X + 1, 5, xorstr_("StuzziKLL UC INTERNAL"), pDevice);
		DrawBorderMenu(11, 48, 99, 29, BorderWhite, pDevice);
		XVPVQW_Menu(X + 20, Y + 48, 99, 27, xorstr_("StuzziKLL UC ESP"), Fun.Menu1, pDevice);
		//XVPVQW_Menu(X + 20, Y + 48 + 20, 99, 27, xorstr_("Aimbot"), Fun.Menu2, pDevice);

        Fun.cLine = 0;
        Fun.cSkeleton = 0;
        Fun.cAbility = 0;
        Fun.cSpike = 0;
		if (Fun.Menu1)
		{
			Draw_Menu(131, 212, xorstr_("Esp Settings"), pDevice);
			XVPVQW(X + 138, 55, 12, 12, Fun.cBox, xorstr_("  Esp Box"), pDevice);
			XVPVQW(X + 138, 55 + 15, 12, 12, Fun.cHead, xorstr_("  Esp Head"), pDevice);
			XVPVQW(X + 138, 55 + 15 * 2, 12, 12, Fun.cHealthbar, xorstr_("  Esp Healthbar"), pDevice);
			XVPVQW(X + 138, 55 + 15 * 3, 12, 12, Fun.cDistance, xorstr_("  Esp Distance"), pDevice);
			XVPVQW(X + 138, 55 + 15 * 4, 12, 12, Fun.cCharacter, xorstr_("  Esp Player"), pDevice);
            XVPVQW(X + 138, 55 + 15 * 5, 12, 12, Fun.cRadar, xorstr_("  Draw Radar"), pDevice);
		}
		if (Fun.Menu2)
		{
			Draw_Menu(161, 305, xorstr_("Aimbot Settings"), pDevice);
			XVPVQW(X + 138, 55, 12, 12, Fun.cAim, xorstr_("  Aimbot"), pDevice);
			XVPVQW(X + 138, 55 + 15, 12, 12, Fun.cAimDrone, xorstr_("  Aimbot Drone & Camera"), pDevice);
			XVPVQW(X + 138, 55 + 15 * 2, 12, 12, Fun.cRCS, xorstr_("  Recoil Compensation"), pDevice);

			Scrol(X + 145, 55 + 15 * 4, Fun.Scroll_Function1, optBone, 2, xorstr_("  Bone"), pDevice);
			Scrol(X + 145, 55 + 15 * 5, Fun.Scroll_Function2, optKey, 10, xorstr_("  Key"), pDevice);
			Scrol(X + 145, 55 + 15 * 6, Fun.Scroll_Function3, optFov, 8, xorstr_("  Fov"), pDevice);
			Scrol(X + 145, 55 + 15 * 7, Fun.Scroll_Function4, optSmooth, 10, xorstr_("  Smooth"), pDevice);
		}
	}
}

void SetWindowToTarget()
{
	std::string c1 = ("lWi");
	std::string c2 = ("ndow");
	std::string c3 = ("Unrea") + c1 + c2;
    GameWnd = FindWindow(c3.c_str(), 0);
    RtlSecureZeroMemory(&c3, sizeof(c3));
    
    ZeroMemory(&GameRect, sizeof(GameRect));
    GetWindowRect(GameWnd, &GameRect);
    Width = GameRect.right - GameRect.left;
    Height = GameRect.bottom - GameRect.top;
    
    DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
    if (dwStyle & WS_BORDER)
    {
        GameRect.top += 34;
        Height -= 37;
    }
    
    ScreenCenterX = Width / 2;
    ScreenCenterY = Height / 2;
    
}

void abc() {
    int xsd = 441;
}
D3DPRESENT_PARAMETERS d3dpp;
void render()
{
    abc();
    d3ddev->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, NULL);
    d3ddev->BeginScene();
    // RtlSecureZeroMemory(&wnd_str, sizeof(wnd_str));

    if (GameWnd == GetForegroundWindow() || GetActiveWindow() == GetForegroundWindow())
    {
        abc(); abc();
        Menu(d3ddev); abc(); abc();
        EspLoop(); abc(); abc();
    }
    abc();
    d3ddev->EndScene();
    HRESULT c3 = d3ddev->Present(NULL, NULL, NULL, NULL);


    if (c3 != 0)
    {
        d3d->CreateDevice(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            MyWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &d3ddev);

        D3DXCreateLine(d3ddev, &S_Line);
        PostReset(d3ddev);
        d3d->Release();//discharge
    }
}
void CleanuoD3D()
{
    if (d3ddev != NULL)
    {
        d3ddev->EndScene();
        d3ddev->Release();
    }
    if (d3d != NULL)
    {
        d3d->Release();
    }
}
WPARAM MainLoop()
{
    
    static RECT old_rc;
    ZeroMemory(&Message, sizeof(MSG));
    while (Message.message != WM_QUIT)
    {
        
        render();
        if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
        {
            
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
       /* HWND hwnd_active = GetForegroundWindow();
        
        if (GetAsyncKeyState(0x23) & 1)
            exit(8);
        
        if (hwnd_active == GameWnd) {
            HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
            SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }*/
        
    }
    
    CleanuoD3D();
    DestroyWindow(MyWnd);
    ExitProcess(0);
    return Message.wParam;
    
}

bool initD3D(HWND hWnd)
{
    
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    
    d3dpp.hDeviceWindow = hWnd; 
    d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE; 
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
    d3dpp.BackBufferCount = 1; 
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; 
    d3dpp.BackBufferWidth = 0; 
    d3dpp.BackBufferHeight = 0; 
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; 
    d3dpp.Windowed = true; 
    d3dpp.EnableAutoDepthStencil = TRUE; 
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3ddev);

    D3DXCreateLine(d3ddev, &S_Line);
    PostReset(d3ddev);
    d3d->Release();//discharge
    
    return true;
    
}


void SetupDX()
{   
    auto ahpxp = xorstr_("d3d9.dll");
    while (!GetModuleHandleA(ahpxp)) {
        Sleep(1);
    }   
    RtlSecureZeroMemory(&ahpxp, sizeof(ahpxp));

    const char* wnd_str2 = xorstr_("UCVALO");
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = 0;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wnd_str2;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    RegisterClassEx(&wc);
    
    SetWindowToTarget();
  

    MyWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        wnd_str2,
        "",
        WS_POPUP | WS_VISIBLE,
        GameRect.left, GameRect.top, Width, Height,
        NULL, NULL, 0, NULL);

    RtlSecureZeroMemory(&wnd_str2, sizeof(wnd_str2));

    SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    DwmExtendFrameIntoClientArea(MyWnd, &Margin);
    HANDLE pp = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cache, 0, 0, 0);
    CloseHandle(pp);
    
    if (initD3D(MyWnd))
    {
        HANDLE pp2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainLoop, 0, 0, 0);
        CloseHandle(pp2);

    }
    
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch (message)
    {
    case WM_PAINT:
    {
    DwmExtendFrameIntoClientArea(hWnd, &Margin);
    }
    break;

    case WM_DESTROY:
    {
        CleanuoD3D();
        PostQuitMessage(0);
        exit(4);
        return 0;
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return 0;
}


