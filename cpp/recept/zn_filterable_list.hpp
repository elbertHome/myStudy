#pragma once

template<typename T>
class zn_filter
{
	private:
	public:
		virtual bool perform(std::vector<T> &entries) = 0;
};

template<typename T>
class zn_filterable_list
{
	protected:
		std::vector<T> _entries;
		std::vector<zn_filter<T> *> _filters;

	public:
		// normal functions
		void add_entry(const T &entry)
		{
			_entries.push_back(entry);
		}
		T &last_entry()
		{
			return _entries.back();
		}

		// iterator
		typedef typename std::vector<T>::iterator iterator;
		iterator begin()
		{
			return _entries.begin();
		}
		iterator end()
		{
			return _entries.end();
		}

		// filter
		void add_filter(zn_filter<T> *filter)
		{
			_filters.push_back(filter);
		}
		bool perform_filter()
		{
			for (typename std::vector<zn_filter<T> *>::iterator it = _filters.begin();
					it != _filters.end();
					++it)
			{
				(*it)->perform(_entries);
			}

			_filters.clear();
			return true;
		}
};
