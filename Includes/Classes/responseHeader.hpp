#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "std_libs.hpp"
// # include "constantes.hpp"
# include "utils.hpp"

class ResponseHeader
{
	public:
		ResponseHeader();
		~ResponseHeader();
		ResponseHeader(const ResponseHeader &copy);
		ResponseHeader &operator=(const ResponseHeader &other);
		
		void setContentType(const std::string &type);
		void setContentLength(const std::string &length);
		void setDate(const std::string &date);
		void setLastModified(const std::string &last);
		void setServer(const std::string &server);
		void setLocation(const std::string &location);

		void setValues(const std::string &type, const std::string &length, const std::string &date, const std::string &last, const std::string &location);
		
		std::string transformHeaderToString(int statusCode, std::string type, std::string length, std::string date, std::string last, std::string location);

	private:
		// Type de contenu de la réponse
		std::string	_contentType;
		// Taille du body de la réponse
		std::string	_contentLength;
		// Date et heure à laquelle la réponse a été généré
		std::string	_date;
		// Date et heure de la dernière modification du contenu de la ressource
		std::string	_lastModified;
		// Nom du logiciel serveur
		std::string _server;
		// Utilisé lors d'une redirection pour indiquer l'URL vers laquelle le client doit être redirigé
		std::string _location;

		std::string getHeaderData();
		void initValues();
};

#endif // RESPONSEHEADER_HPP