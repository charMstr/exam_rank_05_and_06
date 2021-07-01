/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fwoosh.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:05:51 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:07:32 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fwoosh.hpp"

//constructor
Fwoosh::Fwoosh(void) : ASpell("Fwoosh", "fwooshed")
{}

Fwoosh *
Fwoosh::clone(void) const
{
	return (new Fwoosh());	
}
