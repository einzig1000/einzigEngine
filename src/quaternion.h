
struct quaternion
{
    float x = 0, y = 0, z = 0, w = 1;

    quaternion operator*(const quaternion& rhs) const
    {
        return quaternion{
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
        };
    }
    quaternion operator*(float scalar) const
    {
        return quaternion{ x * scalar, y * scalar, z * scalar, w * scalar };
	}

    // 単位quaternion
    static quaternion MakeIdentityQuaternion();
    // 共役quaternion
	static quaternion MakeConjugateQuaternion(const quaternion& q);
    // quaternionのnormを返す
	static float Norm(const quaternion& q);
    // 正規化したquaternion
    static quaternion Normalize(const quaternion& q);
    // 逆quaternion
    static quaternion Inverse(const quaternion& q);
};
