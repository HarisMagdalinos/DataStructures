#include "common.h"
#include "dynamic_array.h"
#define INITIAL_INDEX_CAPACITY 256
#define INITIAL_BUCKET_CAPACITY 4




template <typename KeyType,typename ValueType>
class Hash
{

public:
	/// \brief ValueSet Type of set of values.
	typedef dynamic_array<ValueType> ValueSet;

protected:
	/// \brief The Bucket struct represents a bucket.Each bucket holds its local depth, a primary key and a set of values
    	/// which are offsets to the respective Journal of the relation.
	struct Bucket {
	        /// \brief The bucket's local depth
	        length_t localDepth;
	        /// \brief The bucket's primary key
       		KeyType key;
	        /// \brief The bucket's set of offsets to the Journal.
	        ValueSet values;


	        /// \brief Bucket's constructor.Calls constructors of members.
    		/// \param depth Local depth of the bucket to be constructed.
        	/// \param capacity Indicates max capacity of set of values.
        	Bucket(length_t depth, length_t capacity)
            	: values(capacity), localDepth(depth)
        	{}


        	/// \brief print Prints all members of a bucket for test purposes.
        	void print() const
 	     	{
        		std::cout << "(" << localDepth << ") {" << key << "} [ ";
           		for (length_t i = 0; i < values.size(); i++) 
			{
//                	 	std::cout << values[i] << std::endl;
            		}
            		std::cout << "]" << std::endl;
        	}
    	};

public:		
	Hash()
	  :m_globalDepth(1),m_bucketsAtNextMax(0),
	   m_bucketsAtMaxDepth(1 << m_globalDepth),
	   m_buckets(INITIAL_INDEX_CAPACITY),m_index(INITIAL_INDEX_CAPACITY)
	  {
		length_t index_size = 1 << m_globalDepth;
		length_t index;
		for(length_t i = 0; i < index_size; i++)
		{
			index=newBucket(m_globalDepth,INITIAL_BUCKET_CAPACITY);
			m_index.push_back(index);	
		}
	  }
		
	Hash(length_t globalDepth)
	  :m_globalDepth(globalDepth),m_bucketsAtNextMax(0),
	   m_bucketsAtMaxDepth(1 << m_globalDepth),
	   m_buckets(INITIAL_INDEX_CAPACITY),m_index(INITIAL_INDEX_CAPACITY)
	  {
		length_t index_size = 1 << m_globalDepth;
		length_t index;
		for(length_t i = 0; i < index_size; i++)
		{
			index=newBucket(m_globalDepth,INITIAL_BUCKET_CAPACITY);
			m_index.push_back(index);	
		}
	  }
	
	~Hash()
	{}
	
	void splitBucket(Bucket &old_bucket)
	{
	    	// We must assign the referance of the old backet to a new object
    		// because the memory of old bucket can potentially be allocated
    		Bucket bucket = old_bucket;
    		old_bucket.localDepth++;
    		// Find the first index cell that points to this bucket.
		if(old_bucket.localDepth == m_globalDepth)
		{
			m_bucketsAtMaxDepth += 2;
			m_bucketsAtNextMax -= 2;
		}
		else if(old_bucket.localDepth == m_globalDepth - 1)
		{
			m_bucketsAtNextMax += (1 << (m_globalDepth - (old_bucket.localDepth - 1)));
		}
    		length_t pos, step;
    		findPointers(bucket, pos, step);
    		// Create a new bucket
    		length_t nbi = newBucket(++bucket.localDepth, bucket.values.capacity());
    		// Distribute the index cells among the two buckets.
    		length_t newHashed = maskValue(bucket.key, bucket.localDepth);
    		// Retrace all index cells that were pointing to this bucket, and if they don't
    		// match the hash point them to the empty bucket.
    		while (pos < m_index.size()) 
		{
        		if (maskValue(pos, bucket.localDepth) != newHashed) 
			{
            			m_index[pos] = nbi;
        		}
        		pos += step;
    		}
		
	}

	void insert(KeyType key,ValueType value)
	{
    		// Determine the index cell position by hashing the provided key and keeping the last
    		// m_globalDepth bits of the result.
    		length_t pos = maskValue(key, m_globalDepth);
    		Bucket &bucket = getBucket(pos);
    		// If the bucket pointed by the cell is empty, or the key in the cell is equal to the
    		// one provided, just add the value into the bucket.
    		if (bucket.values.size() == 0 || bucket.key == key) {
        		getBucket(pos).values.push_back(value);
        		bucket.key = key;
    		}
    		else {
        		if (bucket.localDepth < m_globalDepth) {
            			splitBucket(bucket);
        		}
        		else {
            			doubleIndex();
        		}
        		insert(key, value);
    		}
	}

	//Returns a set for values for key key
	ValueSet find(KeyType key) const
	{
		// Determine the index cell position by hashing the provided key and keeping the last
    		// m_globalDepth bits of the result.
    		length_t pos = maskValue(key, m_globalDepth);
    		const Bucket &bucket = getBucket(pos);
    		// If key exists in  bucket found return the values.
    		if (bucket.key == key) {
    		    return bucket.values;
    		}
    		// If not found return empty set.
    		return ValueSet();
	}

	//Remove a value from set of values of bucket with key key.
	void removeValue(KeyType key,ValueType value)
	{
		// Determine the index cell position by hashing the provided key and keeping the last
		// m_globalDepth bits of the result.
		length_t pos = maskValue(key, m_globalDepth);
		Bucket &bucket = getBucket(pos);
    		// Search if the value given exists in the set of values.
    		length_t i = bucket.values.linearSearch(value);
    		// If value exists:
    		if (i < bucket.values.size()) {
        		// Put the last value in the place of the item to delete
        		bucket.values[i] = bucket.values[bucket.values.size() - 1];
        		// And delete the last item.
        		bucket.values.pop_back();
    		}
    		// If all the values of this key have been deleted, remove the key altogether
    		if (bucket.values.size() == 0) {
        		if (bucket.localDepth == m_globalDepth && m_globalDepth > 0) {
            			removeKey(key);
        		}
    		}
	}

	// Check if hash table can be halved in size.
	bool canCollapse()
	{
		bool canCollapse = false;
	    	length_t size = m_index.size();
		if(m_globalDepth > 0)
		{
        		length_t step = 1 << (m_globalDepth - 1);
        		canCollapse=true;
        		// If each bucket has two or more pointers
        		// hash table can be collapsed.
        		for (length_t i = 0; i < size/2; i++)
	    		{
                		if(m_index[i] != m_index[i + step])
                		{
                    			canCollapse=false;
                        		break;
                		}
	  	  	}
		}
		return canCollapse;
	}

	//Remove a bucket with key key.
	void removeKey(KeyType key)
	{
		// Determine the index cell position by hashing the provided key and keeping the last
    		// m_globalDepth bits of the result.
    		length_t pos = maskValue(key, m_globalDepth);
    		// First index to visit.
    		length_t firstPos;
    		// Current+step Indicates the next index to visit.
    		length_t step;
    		// Index of buddy bucket at hash table.
    		length_t buddyPos;
    		firstPos = mergeBucket(pos);
    		// While hash table can be collapsed at half size.
    		//while(canCollapse())
		while(m_bucketsAtMaxDepth == 0)
    		{
        		//collapse hash table at half size.
        		collapse();
        		step = 1 << (m_globalDepth - 1);
        		//Find position of buddy bucket.
        		buddyPos = (firstPos + step) % (1 << m_globalDepth);
        		//get buddy bucket
        		Bucket &buddyBucket = getBucket(buddyPos);
        		//If buddy bucket empty merge again.
        		if(buddyBucket.values.size() == 0)
        		{
            			mergeBucket(buddyPos);
        		}
    		}
	}

	// Merge two buckets.
	length_t mergeBucket(length_t oldPos)
	{
		length_t step = 1 << (m_globalDepth - 1);
    		// Find buddy bucket index.
    		length_t buddyPos = (oldPos + step) % (1 << m_globalDepth);
		// Get bucket to be deleted.
		Bucket &oldBucket = getBucket(oldPos);
		// Find number of bucket to be deleted.
		length_t oldBucketNum = m_index[oldPos];
		length_t bucketsSize = m_buckets.size();
    		// Reduce local depth of merged bucket.
		Bucket &buddyBucket = getBucket(buddyPos);
		buddyBucket.localDepth--;
		// Check if bucket to be deleted is not the last bucket.
		if(oldBucketNum != bucketsSize - 1)
		{
			// Change pointer so that it points to buddy bucket.
    			m_index[oldPos] = m_index[buddyPos];
    			// Copy last bucket at bucket's to be deleted place.
    			oldBucket = m_buckets[bucketsSize - 1];
    			// Delete last bucket.
    			m_buckets.pop_back();
            		// Redirect pointers to lastbucket to its new position
			length_t pos,movedStep;
			findPointers(oldBucket,pos,movedStep);
    			for(length_t i = pos; i < m_index.size(); i += movedStep)
    			{
            			m_index[i] = oldBucketNum;
    			}
		}
		else
		{
			// Change pointer so that it points to buddy bucket.
    			m_index[oldPos] = m_index[buddyPos];
    			// Delete last bucket.
    			m_buckets.pop_back();
		}
		m_bucketsAtMaxDepth -= 2;
		if(buddyBucket.localDepth >= 0)
		{
			if(m_bucketsAtNextMax < (1 << m_globalDepth))
			{
				m_bucketsAtNextMax += 2;
			}
			else
			{
				m_bucketsAtNextMax -= 2;
			}
		}
		else
		{
			m_bucketsAtNextMax = 1;
		}
    		return buddyPos;
	}

	void doubleIndex()
	{
		length_t size = m_index.size();
		for(length_t i = 0; i < size; i++)
		{
			m_index.push_back(m_index[i]);
		}
		m_globalDepth++;
		m_bucketsAtNextMax = 2 * m_bucketsAtMaxDepth;
		m_bucketsAtMaxDepth = 0;
	}
	
	void collapse()
	{
		length_t size = m_index.size()/2;
		for(length_t i = 0; i < size; i++)
		{
			m_index.pop_back();
		}
		m_globalDepth--;
		m_bucketsAtMaxDepth = m_bucketsAtNextMax/2;
		if(m_globalDepth > 1)
		{
			m_bucketsAtNextMax = m_bucketsAtNextMax/2; 
		}
		else
		{
			m_bucketsAtNextMax = 0;
		}
	}

	length_t newBucket(length_t localDepth,length_t capacity)
	{
		Bucket bucket(localDepth,capacity);
		m_buckets.push_back(bucket);
		return m_buckets.size() - 1;
	}

	const Bucket &getBucket(length_t cell_index) const
	{
	    return m_buckets[m_index[cell_index]];
	}

	Bucket &getBucket(length_t index)
	{
		return m_buckets[m_index[index]];
	}

	void findPointers(const Bucket &bucket, length_t &pos, length_t &step) const
	{
 		pos = maskValue(bucket.key, bucket.localDepth);
	    	step = 1 << bucket.localDepth;
	}

	length_t maskValue(KeyType value,length_t depth) const
	{
		return value % (1 << depth);
	}

	void printIndex()
	{
		uint64_t value;
		for(uint64_t i = 0; i < m_index.size(); i++)
		{
			value=m_index[i];
			const Bucket &bucket=getBucket(i);
	    		if(bucket.values.size() != 0)
       			{
               			 bucket.print();
        		}
		}
	}

private:
   	/// \brief m_globalDepth Current global depth of the hash table.
    	length_t m_globalDepth;
	
	length_t m_bucketsAtNextMax;
	
	length_t m_bucketsAtMaxDepth;

    	/// \brief m_buckets Holds the buckets created for the hash table.
    	dynamic_array<Bucket> m_buckets;
    	/// \brief m_index Holds the mapping of the buckets.
    	dynamic_array<length_t> m_index;
};

