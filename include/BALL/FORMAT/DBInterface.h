// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: DBInterface.h,v 1.1 2005/11/06 19:36:33 oliver Exp $
//

#ifndef BALL_FORMAT_DBINTERFACE_H
#define BALL_FORMAT_DBINTERFACE_H

#include <BALL/KERNEL/system.h>
#include <BALL/KERNEL/molecule.h>
#include <BALL/FORMAT/MOLFile.h>
#include <BALL/COMMON/exception.h>

#include <qsqlrecord.h>
#include <qsqldatabase.h>

namespace BALL
{

	/** A simplified interface to a small molecule structure database.
	*/
	class DBInterface
	{
		public:
		class InvalidQuery
			:	public Exception::GeneralException
		{
			public:
			InvalidQuery(const char* file, int line, const String& s = "<unknown query>") throw();
			virtual ~InvalidQuery() throw() {}

			protected:
			std::string query_;
		};

		class NotConnected
			:	public Exception::GeneralException
		{
			public:
			NotConnected(const char* file, int line, const String& s = "<not connected>") throw();
			virtual ~NotConnected() throw() {}

			protected:
			std::string query_;
		};

		/// Exception thrown to indicate that the given topology and conformation are not consistent
		class InconsistentTopology
			:	public Exception::GeneralException
		{
			public:
			InconsistentTopology(const char* file, int line, const String& s = "<inconsistent topology>") throw();
			virtual ~InconsistentTopology() throw() {}

			protected:
			std::string query_;
		};

		/**	@name Type definitions */
		//@{
			
		/// A database ID
		typedef LongSize ID;

		/// A vector of database IDs
		typedef std::vector<ID> IDVector;

		/** Describes a method for conformation generation.
				First: name of the method used, second: the paramters used.
		*/
		typedef std::pair<String, String> ConformationMethod;

		/// Database status codes
		enum ErrorCodes
		{
			NO_ERROR,
			NO_CONNECTION // the database was not connected/initialized (good() = false)
		};

		//@}

		/**	@name Constants */
		//@{

		/// The file containing the login credentials used by connect()
		static const String BALL_DEFAULT_DBRCFILE;
		/// The default database host
		static const String BALL_DEFAULT_DATABASE_HOST;
		/// The default database port
		static const Size   BALL_DEFAULT_DATABASE_PORT;
		/// The default database driver
		static const String BALL_DEFAULT_DATABASE_DRIVER;
		/// The default database name
		static const String BALL_DEFAULT_DATABASE_NAME;
		/// The section name for the INI file containing the login credentials
		static const String BALL_DEFAULT_DATABASE_SECTIONNAME;
		//@}
		

		/**	@name Constructors and destructors */
		//@{
		///
		DBInterface();
		///
		virtual ~DBInterface() {}
		//@}

		/**	@name Create and retrieve topologies and conformations */
		//@{
		/// Retrieve a molecular topology (no coordinates)
		void getTopology(ID topology, System& system);
		/** Create a new topology. If no ID is specified, it will create a new ID.	
				Otherwise, old data could be overwritten. In any case all associated 
				conformations are deleted as well for consistency reasons.
		*/
		ID newTopology(const System& system, const String& name, const String& source_id, ID id = 0);
		/// Replace an existing topology. This will remove all associated conformatins as well.
		void setTopology(ID topology, const System& system);

		/// Return IDs of all conformations for a given topology
		IDVector getConformationList(ID structure);
		/// Return IDs of all conformations for a given topology created with a specific method
		IDVector getConformationList(ID topology_id, ID method_id);
		/// Assign a specific conformation to an existing topology
		void loadConformation(const ID conformation, System& system);
		/// Store the current conformation 
		ID storeConformation(ID topology, ID method_ID, const System& system);

		/// Return IDs for all current methods for conformation generation
		IDVector getConformationMethods();
		/// Return name and parameters of a conformation generation method
		ConformationMethod getConformationMethod(ID method_id);
		/// Return name and parameters of a conformation generation method
		ID getConformationMethod(const String& method, const String& parameters);
		/// Create a new conformation generation method and return its database ID
		ID newConformationMethod(const String& method, const String& parameters);
		//@}
				
		/**	@name Debugging and diagnostics */
		//@{
		///
		ErrorCode getError() const { return error_; }
		///
		void setError(ErrorCode error) { error_ = error; }
		///
		bool good() const { return error_ == 0; }
		//@}

		/**	@name	Simplified database interface */
		//@{
		/** Connect to the database.
		Remember: it is not safe to store clear text passwords in your source code!
		Please use \link connect() connect() \endlink whenever possible.
		\p
		The method returns false, if the database connection could not be established.
		Inspecting query().lastError().text() will help you to identify potential problems.
		*/
		bool connect
			(const String& user, const String& password, 
			 const String& database_name = "structures", const String& host = "diclofenac.informatik.uni-tuebingen.de",
			 Size port = 3306, const String& driver = "QMYSQL3");


		/** Connect to the database using the default login settings.
				The default settings for a user are stored in $HOME/.ballrc.
				Make sure this file is readable for *YOU ONLY* (e.g. by "chmod 600 ~/.ballrc").
				The file adheres to the format definition of an \link INIFile INIFile \endlink.
				The keywords recognized can be seen from the example below:
				\verbatim
					[BALLStructureDatabase]
					user=oliver
					password=mypassword
					database=structures
					host=diclofenac.informatik.uni-tuebingen.de
					port=3306
					driver=QMYSQL3
				\verbatim
				Except for user and passwort the values shown will be used as default values,
				if the corresponding key is not given.
				The method returns false, if the database connection could not be established.
				Inspecting query().lastError().text() will help you to identify potential problems.
		*/
		bool connect();
		
		/// Execute a query
		QSqlQuery& executeQuery(const String& query_string) 
			throw(InvalidQuery, NotConnected);
		
		/// Execute a prepared query
		QSqlQuery& executeQuery() 
			throw(InvalidQuery, NotConnected);

		/// Return the internal query.
		QSqlQuery& query() { return *query_; }

		/// Return the (expanded) last query
		String executedQuery() { return query_->executedQuery().ascii();}

		/// Skip to the first result of the last query.
		bool first() { return query_->first(); }

		/// Skip to the last result of the last query.
		bool last() { return query_->last(); }

		/// Skip to the next result of the last query (if it exists)
		bool next() { return query_->next(); }

		/// Skip to the previous result of the last query (if it exists)
		bool prev() { return query_->prev(); }

		/// Return the last database ID created by an insert statement
		ID lastInsertedID();
		
		/// Return the number of rows returned by the last query
		Size size() { return (Size)query_->size(); }

		/// Return the k-th column value of the current result row
		QVariant value(Position k) { return query_->value(k); }

		/// Prepare a query
		void prepare(const String& s) { query_->prepare(s.c_str()); }

		/// Add bound parameters to the query (replaces ? in a prepared query)
		void addBindValue(const QVariant& v) { query_->addBindValue(v); }
		void addBindValue(const QVariant& v1, const QVariant& v2) { addBindValue(v1); addBindValue(v2);}
		void addBindValue(const QVariant& v1, const QVariant& v2, const QVariant& v3) { addBindValue(v1); addBindValue(v2); addBindValue(v3);}
		void addBindValue(const QVariant& v1, const QVariant& v2, const QVariant& v3, const QVariant& v4) { addBindValue(v1); addBindValue(v2); addBindValue(v3); addBindValue(v4);}
		//@}

		/**	@name Obsolete stuff -- to be removed asap */
		//@{
		/// Retrieve the structure as a single MOLFile (if stored) from the old fingerprints table
		bool getMOLFile(ID id, String& file_text, String& name, String& source_id);
		/// Add the structure from a MOLFile contained in a string to a System
		void addMOLFileToSystem(const String& molfile, System& system);
		//@}

		protected:	

		// Database conection
		ErrorCode				error_;
		QSqlDatabase*		db_;
		QSqlQuery*			query_;
		
		// Connection details
		String					database_name_;
		Size						port_;
		String					host_;
		String					username_;
		String					password_;

		private:
		// No copy construction allowed. Don't know what QSqlDatabase would
		// do in that case. It can't be copied anyhow...
		DBInterface(const DBInterface& /* interface */) {}
	};

} // namespace BALL
#endif // BALL_FORMAT_DBINTERFACE_H