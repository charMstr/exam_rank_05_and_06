/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATarget.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 04:49:13 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:03:00 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATARGET_HPP
# define ATARGET_HPP

#include <string>

class ASpell;

class ATarget
{
	protected:
	//DATA
		std::string type;

	public:	
	//FUNCTIONS:	
		//constructor
		ATarget(void);
		ATarget(const ATarget &src);
		ATarget &operator=(const ATarget &src);

		//constructor
		ATarget(std::string const &type);
		//destructor
		virtual ~ATarget(void);

		std::string const &getType(void) const;
		void 	getHitBySpell(const ASpell &apell) const; 

	virtual ATarget* clone(void) const = 0;
};

#endif
