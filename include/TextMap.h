#pragma once
#ifndef _TEXTMAP_H_
#define _TEXTMAP_H_

#if defined(_DEBUG)
	#define TEXTMAP_COUNTER
#endif

namespace FFe
{
	class TextMap
	{
		bool is_number(const String& data) const { return data.size() && ((data[0] >= '0' && data[0] <= '9') || data[0] == '-' || data[0] == '+'); }
		String text_format(const String& data) { String dformat = data; StringReplace(dformat, "\"", "\"\""); return String("\"" + dformat + "\""); }
	public:
		bool IsEmpty() const { return _node->data.size() ? false : true; }
		bool Has(const String& key) const { return _node->map.find(key) != _node->map.end(); }
		bool Has(const int index) const { return (index >= 0 && index < (int)_node->blocks.size()); }
		int Count() const { return _node->blocks.size(); }

		TextMap Get(const unsigned index) const { return (index >= 0 && index < (int)_node->blocks.size()) ? _node->blocks[index] : TextMap(); }
		TextMap Get(const int index) const { return (index >= 0 && index < (int)_node->blocks.size()) ? _node->blocks[index] : TextMap(); }
		TextMap Get(const String& key) const { auto it = _node->map.find(key); return it != _node->map.end() ? it->second : TextMap(); }

		const String& As(const String& value) const { return IsEmpty() ? value : _node->data; }
		const String As(const char* value) const { return As(String(value)); }
		float As(const float value) const { return IsEmpty() || !is_number(_node->data) ? value : stof(_node->data); }
		int As(const int value) const { return IsEmpty() || !is_number(_node->data) ? value : stoi(_node->data); }
		bool As(const bool value) const { return IsEmpty() ? value : is_number(_node->data) ? !stof(_node->data) : _node->data == "true"; }

		TextMap& Add(const TextMap& textMap) { _node->blocks.push_back(textMap); _node->map[textMap.As("")] = textMap; return _node->blocks.back(); }
		TextMap& Add(const String& value) { return Add(TextMap(value, "")); }
		TextMap& Add(const char* value) { return Add(String(value)); }
		TextMap& Add(const int value) { return Add(TextMap(std::to_string(value), "")); }
		TextMap& Add(const float value) { return Add(TextMap(std::to_string(value), "")); }
		TextMap& Add(const bool value) { return Add(TextMap(value ? "true" : "false", "")); }

		bool Sub(const int index)
		{
			if ((index >= 0 && index < (int)_node->blocks.size()))
			{
				for (auto it = _node->map.begin(); it != _node->map.end(); it++)
				{
					if (it->second._node == _node->blocks[index]._node)
					{
						_node->map.erase(it);
						break;
					}
				}
				_node->blocks.erase(_node->blocks.begin() + index);
				return true;
			}
			return false;
		}
		bool Sub(const String& key)
		{
			auto it = _node->map.find(key);
			if (it != _node->map.end())
			{
				for (size_t i = 0; i < _node->blocks.size(); i++)
				{
					if (it->second._node == _node->blocks[i]._node)
					{
						_node->map.erase(it);
						_node->blocks.erase(_node->blocks.begin() + i);
						return true;
					}
				}
			}
			return false;
		}

		TextMap& Merge(const TextMap& other, const Map<String, String>* defines = nullptr, bool create = false)
		{
			String id = other.As("");
			if (defines && id.size() && id[0] == '#')
			{
				auto it = defines->find(id.substr(1));
				if (it != defines->end())
				{
					id = it->second;
				}
				else
				{
					it = defines->find(id.substr(2));
					if (it != defines->end())
					{
						Read(it->second);
						id = _node->blocks.back().As("");
					}
				}
			}
			TextMap ptm;
			if (create || !Has(id))
			{
				ptm = Add(id);
				create = true;
			}
			else
			{
				ptm = Get(id);
			}
			for (int i = 0; i < other.Count(); i++)
			{
				ptm.Merge(other.Get(i), defines, create);
			}
			return *this;
		}

		TextMap& MergeChild(const TextMap& other, const Map<String, String>* defines = nullptr)
		{
			for (int i = 0; i < other.Count(); i++)
			{
				Merge(other.Get(i), defines);
			}
			return *this;
		}

		TextMap Copy()
		{
			TextMap tm;
			SafeRelease(tm._node);
			tm._node = new Node(*this);
			for (int i = 0; i < this->Count(); i++)
			{
				tm.Merge(this->Get(i), nullptr, true);
			}
			return tm;
		}
		TextMap& CopyFrom(const TextMap& other)
		{
			SafeRelease(_node);
			_node = new Node(other);
			for (int i = 0; i < other.Count(); i++)
			{
				Merge(other.Get(i), nullptr, true);
			}
			return *this;
		}

		inline String WriteData() { return _node->data.find(" ") == String::npos && _node->data.find("\"") == String::npos ? _node->data : text_format(_node->data); }
		void Write(String& textout, int indent = 0)
		{
			String indents;
			for (int i = 0; i < indent; i++) indents += "\t";

			int ni = 0;
			for (size_t i = 0; i < _node->blocks.size(); i++) ni += _node->blocks[i].Count();

			if (ni)
			{
				textout += indents + WriteData() + "\r\n";
				textout += indents + "{" + "\r\n";
				for (size_t i = 0; i < _node->blocks.size(); i++) _node->blocks[i].Write(textout, indent + 1);
				textout += indents + "}" + "\r\n";
			}
			else 
			{
				textout += indents + WriteData() + " { ";
				for (size_t i = 0; i < _node->blocks.size(); i++) textout += _node->blocks[i].WriteData() + " ";
				textout += String("}") + "\r\n";
			}
		}

		void Read(const String& text)
		{
			Parse tp(text, "\r\n\t ");
			while (!tp.IsEnd())
			{
				tp.SkipChars();
				String data = tp.GetString(tp.GetSkipChars() + "{}");
				tp.SkipChars();
				Add(TextMap(data, tp.GetBlock('{', '}')));
				tp.SkipChars();
			}
		}

		String Write()
		{
			String text;
			for (size_t i = 0; i < _node->blocks.size(); i++) _node->blocks[i].Write(text, 0);
			return text;
		}
		inline String ToString() { return Write(); }

		TextMap() : _node(new Node("")) {}
		TextMap(const TextMap& other) : _node(other._node->InstanceAdd()) {}
		TextMap(const String& data, const String& pblock) : _node(new Node(data)) { Read(pblock); }
		TextMap(const String& pblock) : _node(new Node("")) { Read(pblock); }
		~TextMap()
		{
			SafeRelease(_node);
		}

		TextMap& operator = (const TextMap& other)
		{
			SafeRelease(_node);
			_node = other._node->InstanceAdd();
			return *this;
		}

		inline TextMap operator / (const String& key) const { return Get(key); }
		inline TextMap operator / (const char* key) const { return Get(key); }
		inline TextMap operator / (const int index) const { return Get(index); }
		inline TextMap operator / (const unsigned index) const { return Get(index); }
		inline TextMap operator [] (const String& key) const { return Get(key); }
		inline TextMap operator [] (const char* key) const { return Get(key); }
		inline TextMap operator [] (const int index) const { return Get(index); }
		inline TextMap operator [] (const unsigned index) const { return Get(index); }

		inline operator String() const { return As(String("")); }
		inline operator float() const { return As(float(0)); }
		inline operator int() const { return As(int(0)); }
		inline operator bool() const { return As(false); }

		template<typename T> inline T operator >> (const T& value) const { return As(value); }

		inline TextMap& operator + (const String& data) { return Add(data); }
		inline TextMap& operator + (const char* data) { return Add(data); }
		inline TextMap& operator + (const int data) { return Add(data); }
		inline TextMap& operator < (const String& data) { Add(data); return *this; }
		inline TextMap& operator < (const char* data) { Add(data); return *this; }
		inline TextMap& operator < (const int data) { Add(data); return *this; }
		inline TextMap& operator < (const float data) { Add(data); return *this; }
		inline TextMap& operator < (const bool data) { Add(data); return *this; }
		inline TextMap& operator <<(const String& data) { Read(data); return *this; }
		inline TextMap& operator <<(const char* data) { Read(data); return *this; }
		inline TextMap& operator - (const String& data) { Sub(data); return *this; }
		inline TextMap& operator - (const char* data) { Sub(data); return *this; }
		inline TextMap& operator - (const int data) { Sub(data); return *this; }

#if defined(TEXTMAP_COUNTER)
		static const int& CounterRef() { return Counter::Instance().total; }
#endif

	private:
#if defined(TEXTMAP_COUNTER)
		struct Counter : public Singleton<Counter> { int total = 0; };
#endif

		class Parse
		{
			int _pos = 0;
			String _text;
			String _skipChars;
		public:
			Parse(const String& text, const String& skipchars) : _text(text), _skipChars(skipchars) {}

			inline bool IsEnd(int offset = 0) { return (_pos + offset) < (int)_text.size() ? false : true; }
			inline bool IsChar(const char c, int offset = 0) { return (!IsEnd(offset) && _text[_pos + offset] == c); }
			inline char ReadChar(void) { return IsEnd() ? 0 : _text[_pos]; }
			inline const String& GetSkipChars() { return _skipChars; }

			bool SkipChar(const char c, const String &skipChars)
			{
				return skipChars.find(c) == String::npos ? false : true;
			}
			void SkipChars(const String &skipChars)
			{
				for (; !IsEnd(); _pos++)
				{
					if (!IsEnd(1) && _text[_pos] == '/' && _text[_pos + 1] == '/')
					{
						for (; !IsEnd(); _pos++) if (IsChar('\n'))break;
					}
					else if (!SkipChar(ReadChar(), skipChars)) return;
				}
			}
			inline void SkipChars() { SkipChars(_skipChars); }

			String GetWord(const String &skipChars)
			{
				int po = _pos;
				for (; !IsEnd(); _pos++) if (SkipChar(ReadChar(), skipChars)) break;
				return _text.substr(po, _pos - po);
			}
			inline String GetWord() { return GetWord(_skipChars); }

			String GetText(const String &skipChars)
			{
				if (IsChar('"'))
				{
					int po = _pos;
					_pos++;
					String text;
					for (; !IsEnd(); _pos++)
					{
						if (IsChar('"')) if (IsChar('"', 1)) _pos++; else break;
						text += ReadChar();
					}
					_pos++;
					return text;
				}
				else
					return "";
			}
			inline String GetText() { return GetText(_skipChars); }

			String GetString(const String &skipChars)
			{
				if (IsChar('"'))
					return GetText(skipChars);
				else
					return GetWord(skipChars);
			}
			inline String GetString() { return GetString(_skipChars); }

			String GetBlock(const char open, const char close, const String &skipChars)
			{
				if (IsChar(open))
				{
					int po = _pos;
					int n = 0;
					for (; !IsEnd(); _pos++)
						if (IsChar(open))
						{
							n++;
						}
						else if (IsChar(close))
						{
							n--;
							if (!n) break;
						}
					_pos++;
					return _text.substr(po + 1, _pos - po - 2);
				}
				else
					return "";
			}
			inline String GetBlock(const char open, const char close) { return GetBlock(open, close, _skipChars); }
		};

		struct Node : Instance<Node>
		{
			String					data;
			Vector<TextMap>			blocks;
			Map<String, TextMap>	map;

			Node(const String& data) : data(data)
			{
#if defined(TEXTMAP_COUNTER)
				Counter::Instance().total++;
#endif
			}
			~Node()
			{
#if defined(TEXTMAP_COUNTER)
				Counter::Instance().total--;
#endif
				blocks.clear();
				data.clear();
				map.clear();
			}
		};
		Node*	_node;
	};

}

#endif