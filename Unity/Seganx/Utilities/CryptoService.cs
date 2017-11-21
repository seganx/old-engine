using System.IO;

public static class CryptoService
{
    public static string ComputeMD5(byte[] src, string salt)
    {
        var md5 = System.Security.Cryptography.MD5.Create();

        //  create bytes
        var saltBytes = System.Text.Encoding.ASCII.GetBytes(salt);
        var inputBytes = new byte[saltBytes.Length + src.Length];

        // copy bytes
        System.Buffer.BlockCopy(src, 0, inputBytes, 0, src.Length);
        System.Buffer.BlockCopy(saltBytes, 0, inputBytes, src.Length, saltBytes.Length);

        // compute hash
        byte[] hashBytes = md5.ComputeHash(inputBytes);
        var res = new System.Text.StringBuilder();
        for (int i = 0; i < hashBytes.Length; i++)
            res.Append(hashBytes[i].ToString("X2"));
        return res.ToString();
    }

    private static byte[] Encrypt(byte[] data, byte[] key)
    {
        var res = new byte[data.Length];
        for (int i = 0; i < data.Length; ++i)
            res[i] = (byte)(data[i] + key[i % key.Length]);
        return res;
    }

    private static byte[] Decrypt(byte[] data, byte[] key)
    {
        var res = new byte[data.Length];
        for (int i = 0; i < data.Length; ++i)
            res[i] = (byte)(data[i] - key[i % key.Length]);
        return res;
    }

    public static byte[] SaveEncryptFile(string filePath, byte[] key, string salt)
    {
        var src = File.ReadAllBytes(filePath);
        var data = Encrypt(src, key);
        var md5 = ComputeMD5(src, salt);
        File.WriteAllBytes(filePath + "." + md5, data);
        return data;
    }

    public static byte[] SaveDecryptFile(string filePath, byte[] key, string salt)
    {
        var src = File.ReadAllBytes(filePath);
        var data = Decrypt(src, key);
        var md5 = ComputeMD5(data, salt);
        var ext = Path.GetExtension(filePath).Remove(0, 1);
        if (md5 != ext) return null;
        File.WriteAllBytes(filePath.Replace("." + ext, ""), data);
        return data;
    }
}
