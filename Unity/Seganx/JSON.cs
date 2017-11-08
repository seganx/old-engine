//#define USE_SharpZipLib
#if !UNITY_WEBPLAYER
#define USE_FileIO
#endif
/* * * * *
 * A simple JSON Parser / builder
 * ------------------------------
 * 
 * It mainly has been written as a simple JSON parser. It can build a JSON string
 * from the node-tree, or generate a node tree from any valid JSON string.
 * 
 * If you want to use compression when saving to file / stream / B64 you have to include
 * SharpZipLib ( http://www.icsharpcode.net/opensource/sharpziplib/ ) in your project and
 * define "USE_SharpZipLib" at the top of the file
 * 
 * Written by Bunny83 
 * 2012-06-09
 * 
 *
 * Features / attributes:
 * - provides strongly typed node classes and lists / dictionaries
 * - provides easy access to class members / array items / data values
 * - the parser now properly identifies types. So generating JSON with this framework should work.
 * - only double quotes (") are used for quoting strings.
 * - provides "casting" properties to easily convert to / from those types:
 *   int / float / double / bool
 * - provides a common interface for each node so no explicit casting is required.
 * - the parser tries to avoid errors, but if malformed JSON is parsed the result is more or less undefined
 * - It can serialize/deserialize a node tree into/from an experimental compact binary format. It might
 *   be handy if you want to store things in a file and don't want it to be easily modifiable
 * 
 * 
 * 2012-12-17 Update:
 * - Added internal JSONLazyCreator class which simplifies the construction of a JSON tree
 *   Now you can simple reference any item that doesn't exist yet and it will return a JSONLazyCreator
 *   The class determines the required type by it's further use, creates the type and removes itself.
 * - Added binary serialization / deserialization.
 * - Added support for BZip2 zipped binary format. Requires the SharpZipLib ( http://www.icsharpcode.net/opensource/sharpziplib/ )
 *   The usage of the SharpZipLib library can be disabled by removing or commenting out the USE_SharpZipLib define at the top
 * - The serializer uses different types when it comes to store the values. Since my data values
 *   are all of type string, the serializer will "try" which format fits best. The order is: int, float, double, bool, string.
 *   It's not the most efficient way but for a moderate amount of data it should work on all platforms.
 * 
 * 2017-03-08 Update:
 * - Optimised parsing by using a StringBuilder for token. This prevents performance issues when large
 *   string data fields are contained in the json data.
 * - Finally refactored the badly named JSONClass into JSONObject.
 * - Replaced the old JSONData class by distict typed classes ( JSONString, JSONNumber, JSONBool, JSONNull ) this
 *   allows to propertly convert the node tree back to json without type information loss. The actual value
 *   parsing now happens at parsing time and not when you actually access one of the casting properties.
 * 
 * 2017-04-11 Update:
 * - Fixed parsing bug where empty string values have been ignored.
 * - Optimised "ToString" by using a StringBuilder internally. This should heavily improve performance for large files
 * - Changed the overload of "ToString(string aIndent)" to "ToString(int aIndent)"
 * 
 * The MIT License (MIT)
 * 
 * Copyright (c) 2012-2017 Markus Göbel
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * * * * */
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SeganX
{
    public static class Json
    {
        public enum NodeType
        {
            Array = 1,
            Object = 2,
            String = 3,
            Number = 4,
            NullValue = 5,
            Boolean = 6,
            None = 7,
        }
        public enum TextMode
        {
            Compact,
            Indent
        }

        public static Node Parse(string aJSON)
        {
            return Node.Parse(aJSON);
        }

        public abstract partial class Node
        {
            #region common interface

            public string Name = "";

            public virtual Node this[int aIndex] { get { return null; } set { } }

            public virtual Node this[string aKey] { get { return null; } set { } }

            public virtual string Value { get { return ""; } set { } }

            public virtual int Count { get { return 0; } }

            public virtual bool IsNumber { get { return false; } }
            public virtual bool IsString { get { return false; } }
            public virtual bool IsBoolean { get { return false; } }
            public virtual bool IsNull { get { return false; } }
            public virtual bool IsArray { get { return false; } }
            public virtual bool IsObject { get { return false; } }

            public virtual void Add(string aKey, Node aItem)
            {
            }
            public virtual void Add(Node aItem)
            {
                Add("", aItem);
            }

            public virtual Node Remove(string aKey)
            {
                return null;
            }

            public virtual Node Remove(int aIndex)
            {
                return null;
            }

            public virtual Node Remove(Node aNode)
            {
                return aNode;
            }

            public virtual IEnumerable<Node> Children
            {
                get
                {
                    yield break;
                }
            }

            public IEnumerable<Node> DeepChildren
            {
                get
                {
                    foreach (var C in Children)
                        foreach (var D in C.DeepChildren)
                            yield return D;
                }
            }

            public override string ToString()
            {
                StringBuilder sb = new StringBuilder();
                WriteToStringBuilder(sb, 0, 0, TextMode.Compact);
                return sb.ToString();
            }

            public virtual string ToString(int aIndent)
            {
                StringBuilder sb = new StringBuilder();
                WriteToStringBuilder(sb, 0, aIndent, TextMode.Indent);
                return sb.ToString();
            }
            internal abstract void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode);

            #endregion common interface

            #region typecasting properties

            public abstract NodeType Tag { get; }

            public virtual double AsDouble
            {
                get
                {
                    double v = 0.0;
                    if (double.TryParse(Value, out v))
                        return v;
                    return 0.0;
                }
                set
                {
                    Value = value.ToString();
                }
            }

            public virtual int AsInt
            {
                get { return (int)AsDouble; }
                set { AsDouble = value; }
            }

            public virtual float AsFloat
            {
                get { return (float)AsDouble; }
                set { AsDouble = value; }
            }

            public virtual bool AsBool
            {
                get
                {
                    bool v = false;
                    if (bool.TryParse(Value, out v))
                        return v;
                    return !string.IsNullOrEmpty(Value);
                }
                set
                {
                    Value = (value) ? "true" : "false";
                }
            }

            public virtual Array AsArray
            {
                get
                {
                    return this as Array;
                }
            }

            public virtual Object AsObject
            {
                get
                {
                    return this as Object;
                }
            }


            #endregion typecasting properties

            #region operators

            public static implicit operator Node(string s)
            {
                return new String(s);
            }
            public static implicit operator string(Node d)
            {
                return (d == null) ? null : d.Value;
            }

            public static implicit operator Node(double n)
            {
                return new Number(n);
            }
            public static implicit operator double(Node d)
            {
                return (d == null) ? 0 : d.AsDouble;
            }

            public static implicit operator Node(float n)
            {
                return new Number(n);
            }
            public static implicit operator float(Node d)
            {
                return (d == null) ? 0 : d.AsFloat;
            }

            public static implicit operator Node(int n)
            {
                return new Number(n);
            }
            public static implicit operator int(Node d)
            {
                return (d == null) ? 0 : d.AsInt;
            }

            public static implicit operator Node(bool b)
            {
                return new Bool(b);
            }
            public static implicit operator bool(Node d)
            {
                return (d == null) ? false : d.AsBool;
            }

            public static bool operator ==(Node a, object b)
            {
                if (ReferenceEquals(a, b))
                    return true;
                bool aIsNull = a is Null || ReferenceEquals(a, null) || a is LazyCreator;
                bool bIsNull = b is Null || ReferenceEquals(b, null) || b is LazyCreator;
                if (aIsNull && bIsNull)
                    return true;
                return a.Equals(b);
            }

            public static bool operator !=(Node a, object b)
            {
                return !(a == b);
            }

            public override bool Equals(object obj)
            {
                return ReferenceEquals(this, obj);
            }

            public override int GetHashCode()
            {
                return base.GetHashCode();
            }

            #endregion operators
            internal static StringBuilder m_EscapeBuilder = new StringBuilder();
            internal static string Escape(string aText)
            {
                m_EscapeBuilder.Length = 0;
                if (m_EscapeBuilder.Capacity < aText.Length + aText.Length / 10)
                    m_EscapeBuilder.Capacity = aText.Length + aText.Length / 10;
                foreach (char c in aText)
                {
                    switch (c)
                    {
                        case '\\':
                            m_EscapeBuilder.Append("\\\\");
                            break;
                        case '\"':
                            m_EscapeBuilder.Append("\\\"");
                            break;
                        case '\n':
                            m_EscapeBuilder.Append("\\n");
                            break;
                        case '\r':
                            m_EscapeBuilder.Append("\\r");
                            break;
                        case '\t':
                            m_EscapeBuilder.Append("\\t");
                            break;
                        case '\b':
                            m_EscapeBuilder.Append("\\b");
                            break;
                        case '\f':
                            m_EscapeBuilder.Append("\\f");
                            break;
                        default:
                            m_EscapeBuilder.Append(c);
                            break;
                    }
                }
                string result = m_EscapeBuilder.ToString();
                m_EscapeBuilder.Length = 0;
                return result;
            }

            static void ParseElement(Node ctx, string token, string tokenName, bool quoted)
            {
                if (quoted)
                {
                    ctx.Add(tokenName, token);
                    return;
                }
                string tmp = token.ToLower();
                if (tmp == "false" || tmp == "true")
                    ctx.Add(tokenName, tmp == "true");
                else if (tmp == "null")
                    ctx.Add(tokenName, null);
                else
                {
                    double val;
                    if (double.TryParse(token, out val))
                        ctx.Add(tokenName, val);
                    else
                        ctx.Add(tokenName, token);
                }
            }

            public static Node Parse(string aJSON)
            {
                Stack<Node> stack = new Stack<Node>();
                Node ctx = null;
                int i = 0;
                StringBuilder Token = new StringBuilder();
                string TokenName = "";
                bool QuoteMode = false;
                bool TokenIsQuoted = false;
                while (i < aJSON.Length)
                {
                    switch (aJSON[i])
                    {
                        case '{':
                            if (QuoteMode)
                            {
                                Token.Append(aJSON[i]);
                                break;
                            }
                            stack.Push(new Object());
                            if (ctx != null)
                            {
                                ctx.Add(TokenName, stack.Peek());
                            }
                            TokenName = "";
                            Token.Length = 0;
                            ctx = stack.Peek();
                            break;

                        case '[':
                            if (QuoteMode)
                            {
                                Token.Append(aJSON[i]);
                                break;
                            }

                            stack.Push(new Array());
                            if (ctx != null)
                            {
                                ctx.Add(TokenName, stack.Peek());
                            }
                            TokenName = "";
                            Token.Length = 0;
                            ctx = stack.Peek();
                            break;

                        case '}':
                        case ']':
                            if (QuoteMode)
                            {

                                Token.Append(aJSON[i]);
                                break;
                            }
                            if (stack.Count == 0)
                                throw new System.Exception("JSON Parse: Too many closing brackets");

                            stack.Pop();
                            if (Token.Length > 0 || TokenIsQuoted)
                            {
                                ParseElement(ctx, Token.ToString(), TokenName, TokenIsQuoted);
                                TokenIsQuoted = false;
                            }
                            TokenName = "";
                            Token.Length = 0;
                            if (stack.Count > 0)
                                ctx = stack.Peek();
                            break;

                        case ':':
                            if (QuoteMode)
                            {
                                Token.Append(aJSON[i]);
                                break;
                            }
                            TokenName = Token.ToString();
                            Token.Length = 0;
                            TokenIsQuoted = false;
                            break;

                        case '"':
                            QuoteMode ^= true;
                            TokenIsQuoted |= QuoteMode;
                            break;

                        case ',':
                            if (QuoteMode)
                            {
                                Token.Append(aJSON[i]);
                                break;
                            }
                            if (Token.Length > 0 || TokenIsQuoted)
                            {
                                ParseElement(ctx, Token.ToString(), TokenName, TokenIsQuoted);
                                TokenIsQuoted = false;
                            }
                            TokenName = "";
                            Token.Length = 0;
                            TokenIsQuoted = false;
                            break;

                        case '\r':
                        case '\n':
                            break;

                        case ' ':
                        case '\t':
                            if (QuoteMode)
                                Token.Append(aJSON[i]);
                            break;

                        case '\\':
                            ++i;
                            if (QuoteMode)
                            {
                                char C = aJSON[i];
                                switch (C)
                                {
                                    case 't':
                                        Token.Append('\t');
                                        break;
                                    case 'r':
                                        Token.Append('\r');
                                        break;
                                    case 'n':
                                        Token.Append('\n');
                                        break;
                                    case 'b':
                                        Token.Append('\b');
                                        break;
                                    case 'f':
                                        Token.Append('\f');
                                        break;
                                    case 'u':
                                        {
                                            string s = aJSON.Substring(i + 1, 4);
                                            Token.Append((char)int.Parse(
                                                s,
                                                System.Globalization.NumberStyles.AllowHexSpecifier));
                                            i += 4;
                                            break;
                                        }
                                    default:
                                        Token.Append(C);
                                        break;
                                }
                            }
                            break;

                        default:
                            Token.Append(aJSON[i]);
                            break;
                    }
                    ++i;
                }
                if (QuoteMode)
                {
                    throw new System.Exception("JSON Parse: Quotation marks seems to be messed up.");
                }
                return ctx;
            }

            public virtual void Serialize(System.IO.BinaryWriter aWriter)
            {
            }

            public void SaveToStream(System.IO.Stream aData)
            {
                var W = new System.IO.BinaryWriter(aData);
                Serialize(W);
            }

#if USE_SharpZipLib
		public void SaveToCompressedStream(System.IO.Stream aData)
		{
			using (var gzipOut = new ICSharpCode.SharpZipLib.BZip2.BZip2OutputStream(aData))
			{
				gzipOut.IsStreamOwner = false;
				SaveToStream(gzipOut);
				gzipOut.Close();
			}
		}
 
		public void SaveToCompressedFile(string aFileName)
		{
 
#if USE_FileIO
			System.IO.Directory.CreateDirectory((new System.IO.FileInfo(aFileName)).Directory.FullName);
			using(var F = System.IO.File.OpenWrite(aFileName))
			{
				SaveToCompressedStream(F);
			}
 
#else
			throw new Exception("Can't use File IO stuff in the webplayer");
#endif
		}
		public string SaveToCompressedBase64()
		{
			using (var stream = new System.IO.MemoryStream())
			{
				SaveToCompressedStream(stream);
				stream.Position = 0;
				return System.Convert.ToBase64String(stream.ToArray());
			}
		}
 
#else
            public void SaveToCompressedStream(System.IO.Stream aData)
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }

            public void SaveToCompressedFile(string aFileName)
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }

            public string SaveToCompressedBase64()
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }
#endif

            public void SaveToFile(string aFileName)
            {
#if USE_FileIO
                System.IO.Directory.CreateDirectory((new System.IO.FileInfo(aFileName)).Directory.FullName);
                using (var F = System.IO.File.OpenWrite(aFileName))
                {
                    SaveToStream(F);
                }
#else
			throw new Exception ("Can't use File IO stuff in the webplayer");
#endif
            }

            public string SaveToBase64()
            {
                using (var stream = new System.IO.MemoryStream())
                {
                    SaveToStream(stream);
                    stream.Position = 0;
                    return System.Convert.ToBase64String(stream.ToArray());
                }
            }

            public static Node Deserialize(System.IO.BinaryReader aReader)
            {
                NodeType type = (NodeType)aReader.ReadByte();
                switch (type)
                {
                    case NodeType.Array:
                        {
                            int count = aReader.ReadInt32();
                            Array tmp = new Array();
                            for (int i = 0; i < count; i++)
                                tmp.Add(Deserialize(aReader));
                            return tmp;
                        }
                    case NodeType.Object:
                        {
                            int count = aReader.ReadInt32();
                            Object tmp = new Object();
                            for (int i = 0; i < count; i++)
                            {
                                string key = aReader.ReadString();
                                var val = Deserialize(aReader);
                                tmp.Add(key, val);
                            }
                            return tmp;
                        }
                    case NodeType.String:
                        {
                            return new String(aReader.ReadString());
                        }
                    case NodeType.Number:
                        {
                            return new Number(aReader.ReadDouble());
                        }
                    case NodeType.Boolean:
                        {
                            return new Bool(aReader.ReadBoolean());
                        }
                    case NodeType.NullValue:
                        {
                            return new Null();
                        }
                    default:
                        {
                            throw new System.Exception("Error deserializing JSON. Unknown tag: " + type);
                        }
                }
            }

#if USE_SharpZipLib
		public static Node LoadFromCompressedStream(System.IO.Stream aData)
		{
			var zin = new ICSharpCode.SharpZipLib.BZip2.BZip2InputStream(aData);
			return LoadFromStream(zin);
		}
		public static Node LoadFromCompressedFile(string aFileName)
		{
#if USE_FileIO
			using(var F = System.IO.File.OpenRead(aFileName))
			{
				return LoadFromCompressedStream(F);
			}
#else
			throw new Exception("Can't use File IO stuff in the webplayer");
#endif
		}
		public static Node LoadFromCompressedBase64(string aBase64)
		{
			var tmp = System.Convert.FromBase64String(aBase64);
			var stream = new System.IO.MemoryStream(tmp);
			stream.Position = 0;
			return LoadFromCompressedStream(stream);
		}
#else
            public static Node LoadFromCompressedFile(string aFileName)
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }

            public static Node LoadFromCompressedStream(System.IO.Stream aData)
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }

            public static Node LoadFromCompressedBase64(string aBase64)
            {
                throw new System.Exception("Can't use compressed functions. You need include the SharpZipLib and uncomment the define at the top of SimpleJSON");
            }
#endif

            public static Node LoadFromStream(System.IO.Stream aData)
            {
                using (var R = new System.IO.BinaryReader(aData))
                {
                    return Deserialize(R);
                }
            }

            public static Node LoadFromFile(string aFileName)
            {
#if USE_FileIO
                using (var F = System.IO.File.OpenRead(aFileName))
                {
                    return LoadFromStream(F);
                }
#else
			throw new Exception ("Can't use File IO stuff in the webplayer");
#endif
            }

            public static Node LoadFromBase64(string aBase64)
            {
                var tmp = System.Convert.FromBase64String(aBase64);
                var stream = new System.IO.MemoryStream(tmp);
                stream.Position = 0;
                return LoadFromStream(stream);
            }
        }
        // End of JSONNode

        public class Array : Node, IEnumerable
        {
            private List<Node> m_List = new List<Node>();
            public bool inline = false;

            public override NodeType Tag { get { return NodeType.Array; } }
            public override bool IsArray { get { return true; } }

            public override Node this[int aIndex]
            {
                get
                {
                    if (aIndex < 0 || aIndex >= m_List.Count)
                        return new LazyCreator(this);
                    return m_List[aIndex];
                }
                set
                {
                    if (value == null)
                        value = new Null();
                    if (aIndex < 0 || aIndex >= m_List.Count)
                        m_List.Add(value);
                    else
                        m_List[aIndex] = value;
                }
            }

            public override Node this[string aKey]
            {
                get { return new LazyCreator(this); }
                set
                {
                    if (value == null)
                        value = new Null();
                    m_List.Add(value);
                }
            }

            public override int Count
            {
                get { return m_List.Count; }
            }

            public override void Add(string aKey, Node aItem)
            {
                if (aItem == null)
                    aItem = new Null();
                m_List.Add(aItem);
            }

            public override Node Remove(int aIndex)
            {
                if (aIndex < 0 || aIndex >= m_List.Count)
                    return null;
                Node tmp = m_List[aIndex];
                m_List.RemoveAt(aIndex);
                return tmp;
            }

            public override Node Remove(Node aNode)
            {
                m_List.Remove(aNode);
                return aNode;
            }

            public override IEnumerable<Node> Children
            {
                get
                {
                    foreach (Node N in m_List)
                        yield return N;
                }
            }

            public IEnumerator GetEnumerator()
            {
                foreach (Node N in m_List)
                    yield return N;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.Array);
                aWriter.Write(m_List.Count);
                for (int i = 0; i < m_List.Count; i++)
                {
                    m_List[i].Serialize(aWriter);
                }
            }

            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append('[');
                int count = m_List.Count;
                if (inline)
                    aMode = TextMode.Compact;
                for (int i = 0; i < count; i++)
                {
                    if (i > 0)
                        aSB.Append(',');
                    if (aMode == TextMode.Indent)
                        aSB.AppendLine();

                    if (aMode == TextMode.Indent)
                        aSB.Append(' ', aIndent + aIndentInc);
                    m_List[i].WriteToStringBuilder(aSB, aIndent + aIndentInc, aIndentInc, aMode);
                }
                if (aMode == TextMode.Indent)
                    aSB.AppendLine().Append(' ', aIndent);
                aSB.Append(']');
            }
        }
        // End of JSONArray

        public class Object : Node, IEnumerable
        {
            private Dictionary<string, Node> m_Dict = new Dictionary<string, Node>();

            public bool inline = false;

            public override NodeType Tag { get { return NodeType.Object; } }
            public override bool IsObject { get { return true; } }


            public override Node this[string aKey]
            {
                get
                {
                    if (m_Dict.ContainsKey(aKey))
                        return m_Dict[aKey];
                    else
                        return new LazyCreator(this, aKey);
                }
                set
                {
                    if (value == null)
                        value = new Null();
                    if (m_Dict.ContainsKey(aKey))
                        m_Dict[aKey] = value;
                    else
                    {
                        value.Name = aKey;
                        m_Dict.Add(aKey, value);
                    }
                }
            }

            public override Node this[int aIndex]
            {
                get
                {
                    if (aIndex < 0 || aIndex >= m_Dict.Count)
                        return null;
                    return m_Dict.ElementAt(aIndex).Value;
                }
                set
                {
                    if (value == null)
                        value = new Null();
                    if (aIndex < 0 || aIndex >= m_Dict.Count)
                        return;
                    string key = m_Dict.ElementAt(aIndex).Key;
                    m_Dict[key] = value;
                    value.Name = key;
                }
            }

            public override int Count
            {
                get { return m_Dict.Count; }
            }

            private static uint counter = 0;
            public override void Add(string aKey, Node aItem)
            {
                if (aItem == null)
                    aItem = new Null();

                if (!string.IsNullOrEmpty(aKey))
                {
                    if (m_Dict.ContainsKey(aKey))
                        m_Dict[aKey] = aItem;
                    else
                        m_Dict.Add(aKey, aItem);
                }
                else
                {
                    counter++;
                    m_Dict.Add(counter.ToString(), aItem);
                }

                aItem.Name = aKey == null ? "" : aKey;
            }

            public override Node Remove(string aKey)
            {
                if (!m_Dict.ContainsKey(aKey))
                    return null;
                Node tmp = m_Dict[aKey];
                m_Dict.Remove(aKey);
                return tmp;
            }

            public override Node Remove(int aIndex)
            {
                if (aIndex < 0 || aIndex >= m_Dict.Count)
                    return null;
                var item = m_Dict.ElementAt(aIndex);
                m_Dict.Remove(item.Key);
                return item.Value;
            }

            public override Node Remove(Node aNode)
            {
                try
                {
                    var item = m_Dict.Where(k => k.Value == aNode).First();
                    m_Dict.Remove(item.Key);
                    return aNode;
                }
                catch
                {
                    return null;
                }
            }

            public override IEnumerable<Node> Children
            {
                get
                {
                    foreach (KeyValuePair<string, Node> N in m_Dict)
                        yield return N.Value;
                }
            }

            public IEnumerator GetEnumerator()
            {
                foreach (KeyValuePair<string, Node> N in m_Dict)
                    yield return N;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.Object);
                aWriter.Write(m_Dict.Count);
                foreach (string K in m_Dict.Keys)
                {
                    aWriter.Write(K);
                    m_Dict[K].Serialize(aWriter);
                }
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append('{');
                bool first = true;
                if (inline)
                    aMode = TextMode.Compact;
                foreach (var k in m_Dict)
                {
                    if (!first)
                        aSB.Append(',');
                    first = false;
                    if (aMode == TextMode.Indent)
                        aSB.AppendLine();
                    if (aMode == TextMode.Indent)
                        aSB.Append(' ', aIndent + aIndentInc);
                    aSB.Append('\"').Append(Escape(k.Key)).Append('\"');
                    if (aMode == TextMode.Compact)
                        aSB.Append(':');
                    else
                        aSB.Append(" : ");
                    k.Value.WriteToStringBuilder(aSB, aIndent + aIndentInc, aIndentInc, aMode);
                }
                if (aMode == TextMode.Indent)
                    aSB.AppendLine().Append(' ', aIndent);
                aSB.Append('}');
            }

        }
        // End of JSONObject

        public class String : Node
        {
            private string m_Data;

            public override NodeType Tag { get { return NodeType.String; } }
            public override bool IsString { get { return true; } }

            public override string Value
            {
                get { return m_Data; }
                set
                {
                    m_Data = value;
                }
            }

            public String(string aData)
            {
                m_Data = aData;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.String);
                aWriter.Write(m_Data);
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append('\"').Append(Escape(m_Data)).Append('\"');
            }
            public override bool Equals(object obj)
            {
                if (base.Equals(obj))
                    return true;
                string s = obj as string;
                if (s != null)
                    return m_Data == s;
                String s2 = obj as String;
                if (s2 != null)
                    return m_Data == s2.m_Data;
                return false;
            }
            public override int GetHashCode()
            {
                return m_Data.GetHashCode();
            }
        }
        // End of JSONString

        public class Number : Node
        {
            private double m_Data;

            public override NodeType Tag { get { return NodeType.Number; } }
            public override bool IsNumber { get { return true; } }


            public override string Value
            {
                get { return m_Data.ToString(); }
                set
                {
                    double v;
                    if (double.TryParse(value, out v))
                        m_Data = v;
                }
            }

            public override double AsDouble
            {
                get { return m_Data; }
                set { m_Data = value; }
            }

            public Number(double aData)
            {
                m_Data = aData;
            }

            public Number(string aData)
            {
                Value = aData;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.Number);
                aWriter.Write(m_Data);
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append(m_Data);
            }
            private static bool IsNumeric(object value)
            {
                return value is int || value is uint
                    || value is float || value is double
                    || value is decimal
                    || value is long || value is ulong
                    || value is short || value is ushort
                    || value is sbyte || value is byte;
            }
            public override bool Equals(object obj)
            {
                if (obj == null)
                    return false;
                if (base.Equals(obj))
                    return true;
                Number s2 = obj as Number;
                if (s2 != null)
                    return m_Data == s2.m_Data;
                if (IsNumeric(obj))
                    return System.Convert.ToDouble(obj) == m_Data;
                return false;
            }
            public override int GetHashCode()
            {
                return m_Data.GetHashCode();
            }
        }
        // End of JSONNumber

        public class Bool : Node
        {
            private bool m_Data;

            public override NodeType Tag { get { return NodeType.Boolean; } }
            public override bool IsBoolean { get { return true; } }


            public override string Value
            {
                get { return m_Data.ToString(); }
                set
                {
                    bool v;
                    if (bool.TryParse(value, out v))
                        m_Data = v;
                }
            }
            public override bool AsBool
            {
                get { return m_Data; }
                set { m_Data = value; }
            }

            public Bool(bool aData)
            {
                m_Data = aData;
            }

            public Bool(string aData)
            {
                Value = aData;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.Boolean);
                aWriter.Write(m_Data);
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append((m_Data) ? "true" : "false");
            }
            public override bool Equals(object obj)
            {
                if (obj == null)
                    return false;
                if (obj is bool)
                    return m_Data == (bool)obj;
                return false;
            }
            public override int GetHashCode()
            {
                return m_Data.GetHashCode();
            }
        }
        // End of JSONBool

        public class Null : Node
        {

            public override NodeType Tag { get { return NodeType.NullValue; } }
            public override bool IsNull { get { return true; } }

            public override string Value
            {
                get { return "null"; }
                set { }
            }
            public override bool AsBool
            {
                get { return false; }
                set { }
            }

            public override bool Equals(object obj)
            {
                if (object.ReferenceEquals(this, obj))
                    return true;
                return (obj is Null);
            }
            public override int GetHashCode()
            {
                return 0;
            }

            public override void Serialize(System.IO.BinaryWriter aWriter)
            {
                aWriter.Write((byte)NodeType.NullValue);
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append("null");
            }
        }
        // End of JSONNull

        internal class LazyCreator : Node
        {
            private Node m_Node = null;
            private string m_Key = null;

            public override NodeType Tag { get { return NodeType.None; } }

            public LazyCreator(Node aNode)
            {
                m_Node = aNode;
                m_Key = null;
            }

            public LazyCreator(Node aNode, string aKey)
            {
                m_Node = aNode;
                m_Key = aKey;
            }

            private void Set(Node aVal)
            {
                if (m_Key == null)
                {
                    m_Node.Add(aVal);
                }
                else
                {
                    m_Node.Add(m_Key, aVal);
                }
                m_Node = null; // Be GC friendly.
            }

            public override Node this[int aIndex]
            {
                get
                {
                    return new LazyCreator(this);
                }
                set
                {
                    var tmp = new Array();
                    tmp.Add(value);
                    Set(tmp);
                }
            }

            public override Node this[string aKey]
            {
                get
                {
                    return new LazyCreator(this, aKey);
                }
                set
                {
                    var tmp = new Object();
                    tmp.Add(aKey, value);
                    Set(tmp);
                }
            }

            public override void Add(Node aItem)
            {
                var tmp = new Array();
                tmp.Add(aItem);
                Set(tmp);
            }

            public override void Add(string aKey, Node aItem)
            {
                var tmp = new Object();
                tmp.Add(aKey, aItem);
                Set(tmp);
            }

            public static bool operator ==(LazyCreator a, object b)
            {
                if (b == null)
                    return true;
                return System.Object.ReferenceEquals(a, b);
            }

            public static bool operator !=(LazyCreator a, object b)
            {
                return !(a == b);
            }

            public override bool Equals(object obj)
            {
                if (obj == null)
                    return true;
                return System.Object.ReferenceEquals(this, obj);
            }

            public override int GetHashCode()
            {
                return 0;
            }

            public override int AsInt
            {
                get
                {
                    Number tmp = new Number(0);
                    Set(tmp);
                    return 0;
                }
                set
                {
                    Number tmp = new Number(value);
                    Set(tmp);
                }
            }

            public override float AsFloat
            {
                get
                {
                    Number tmp = new Number(0.0f);
                    Set(tmp);
                    return 0.0f;
                }
                set
                {
                    Number tmp = new Number(value);
                    Set(tmp);
                }
            }

            public override double AsDouble
            {
                get
                {
                    Number tmp = new Number(0.0);
                    Set(tmp);
                    return 0.0;
                }
                set
                {
                    Number tmp = new Number(value);
                    Set(tmp);
                }
            }

            public override bool AsBool
            {
                get
                {
                    Bool tmp = new Bool(false);
                    Set(tmp);
                    return false;
                }
                set
                {
                    Bool tmp = new Bool(value);
                    Set(tmp);
                }
            }

            public override Array AsArray
            {
                get
                {
                    Array tmp = new Array();
                    Set(tmp);
                    return tmp;
                }
            }

            public override Object AsObject
            {
                get
                {
                    Object tmp = new Object();
                    Set(tmp);
                    return tmp;
                }
            }
            internal override void WriteToStringBuilder(StringBuilder aSB, int aIndent, int aIndentInc, TextMode aMode)
            {
                aSB.Append("null");
            }
        }
    }
}