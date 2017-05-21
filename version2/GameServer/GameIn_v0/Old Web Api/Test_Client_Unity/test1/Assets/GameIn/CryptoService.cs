public static class CryptoService
{
    public class CSRandom
    {
        public uint m_seed;
        public byte m_curr;

        public CSRandom(uint seed = 1363)
        {
            m_seed = seed;
            m_curr = 63;
        }

        public byte generate()
        {
            m_curr += (byte)((m_curr * m_seed * 28454642) + (m_seed * 38745674));
            return m_curr > 0 ? m_curr : ++m_curr;
        }
    };

    public static uint Checksum(byte[] data, uint key = 1363)
    {
        if (data == null || data.Length < 1) return 0;
        uint r = 0;
        CSRandom randomer = new CSRandom(key);
        for (uint i = 0; i < data.Length; ++i)
            r += (uint)(data[i] * 0xabcdef12 + randomer.generate() + data[i] + key);
        return r;
    }

    public static byte[] Encrypt(byte[] src, uint key = 1363)
    {
        byte[] res = new byte[src.Length];
        CSRandom randomer = new CSRandom(key);
        for (uint i = 0; i < src.Length; ++i)
            res[i] = (byte)(src[i] + randomer.generate());
        return res;
    }

    public static byte[] Decrypt(byte[] src, uint key = 1363)
    {
        byte[] res = new byte[src.Length];
        CSRandom randomer = new CSRandom(key);
        for (uint i = 0; i < src.Length; ++i)
            res[i] = (byte)(src[i] - randomer.generate());
        return res;
    }
}


